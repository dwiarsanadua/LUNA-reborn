#include "stdafx.h"
#include "fileio.h"

CFileIO::CFileIO()
{
    bError = FALSE;
    m_hFile = hFileNull;
    m_bCloseOnDelete = FALSE;
}

CFileIO::CFileIO(int hFile)
{
    bError = FALSE;
    m_hFile = hFile;
    m_bCloseOnDelete = FALSE;
}

CFileIO::CFileIO(const char* lpszFileName, uint32_t nOpenFlags)
{
    bError = FALSE;
    if (!Open(lpszFileName, nOpenFlags))
        bError = TRUE;
}

CFileIO::~CFileIO()
{
    if (m_hFile != (uint32_t)hFileNull && m_bCloseOnDelete)
        Close();
}

CFileIO* CFileIO::Duplicate() const
{
    CFileIO* pFile = new CFileIO(hFileNull);
    void* hFile;
    if (!::DuplicateHandle(::GetCurrentProcess(), (void*)m_hFile,
        ::GetCurrentProcess(), &hFile, 0, FALSE, DUPLICATE_SAME_ACCESS))
    {
        delete pFile;
        return NULL;
    }
    pFile->m_hFile = reinterpret_cast<uintptr_t>(hFile);
    pFile->m_bCloseOnDelete = m_bCloseOnDelete;
    return pFile;
}

int32_t CFileIO::Open(const char* lpszFileName, uint32_t nOpenFlags)
{
    // CFileIO objects are always binary and CreateFile does not need flag
    nOpenFlags &= ~(uint32_t)typeBinary;

    m_bCloseOnDelete = FALSE;
    m_hFile = (uint32_t)hFileNull;

    // map read/write mode
    uint32_t dwAccess;
    switch (nOpenFlags & 3)
    {
    case modeRead:
        dwAccess = GENERIC_READ;
        break;
    case modeWrite:
        dwAccess = GENERIC_WRITE;
        break;
    case modeReadWrite:
        dwAccess = GENERIC_READ|GENERIC_WRITE;
        break;
    }

    // map share mode
    uint32_t dwShareMode;
    switch (nOpenFlags & 0x70)
    {
        case shareCompat:       // map compatibility mode to exclusive
        case shareExclusive:
            dwShareMode = 0;
            break;
        case shareDenyWrite:
            dwShareMode = FILE_SHARE_READ;
            break;
        case shareDenyRead:
            dwShareMode = FILE_SHARE_WRITE;
            break;
        case shareDenyNone:
            dwShareMode = FILE_SHARE_WRITE|FILE_SHARE_READ;
            break;
    }

    // Note: typeText and typeBinary are used in derived classes only.

    // map modeNoInherit flag
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = NULL;
    sa.bInheritHandle = (nOpenFlags & modeNoInherit) == 0;

    // map creation flags
    uint32_t dwCreateFlag;
    if (nOpenFlags & modeCreate)
        dwCreateFlag = CREATE_ALWAYS;
    else
        dwCreateFlag = OPEN_EXISTING;

    // attempt file creation
    void* hFile = ::CreateFile(lpszFileName, dwAccess, dwShareMode, &sa,
        dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        bError = TRUE;
        return FALSE;
    }
    m_hFile = static_cast<HFILE>(reinterpret_cast<uintptr_t>(hFile));
    m_bCloseOnDelete = TRUE;
    return TRUE;
}

uint32_t CFileIO::Read(void* lpBuf, uint32_t nCount)
{
    if (nCount == 0)
        return 0;   // avoid Win32 "null-read"

    uint32_t dwRead;
    if (!::ReadFile((void*)m_hFile, lpBuf, nCount, &dwRead, NULL))
    {
        bError = TRUE;
        return FILEIO_ERROR;
    }

    return (uint32_t)dwRead;
}

void CFileIO::Write(const void* lpBuf, uint32_t nCount)
{
    if (nCount == 0)
        return;     // avoid Win32 "null-write" option

    uint32_t nWritten;
    if (!::WriteFile((void*)m_hFile, lpBuf, nCount, &nWritten, NULL))
    {
        bError = TRUE;
        return;
    }

    // Win32s will not return an error all the time (usually DISK_FULL)
    if (nWritten != nCount)
    {
        bError = TRUE;
    }
}

int32_t CFileIO::Seek(int32_t lOff, uint32_t nFrom)
{
    uint32_t dwNew = ::SetFilePointer((void*)m_hFile, lOff, NULL, (uint32_t)nFrom);
    if (dwNew  == (uint32_t)-1)
    {
        bError = TRUE;
        return FILEIO_ERROR;
    }

    return dwNew;
}

uint32_t CFileIO::GetPosition() const
{
    uint32_t dwPos = ::SetFilePointer((void*)m_hFile, 0, NULL, FILE_CURRENT);
    if (dwPos  == (uint32_t)-1)
    {
        return FILEIO_ERROR;
    }

    return dwPos;
}

void CFileIO::Flush()
{
    if (m_hFile == (uint32_t)hFileNull)
        return;

    if (!::FlushFileBuffers((void*)m_hFile))
    {
        bError = TRUE;
        return;
    }
}

void CFileIO::Close()
{
    bError = FALSE;
    if (m_hFile != (uint32_t)hFileNull)
        bError = ::CloseHandle((void*)m_hFile);

    m_hFile = hFileNull;
    m_bCloseOnDelete = FALSE;
}

void CFileIO::Abort()
{
    if (m_hFile != (uint32_t)hFileNull)
    {
        // close but ignore errors
        ::CloseHandle((void*)m_hFile);
        m_hFile = (uint32_t)hFileNull;
    }
}

void CFileIO::LockRange(uint32_t dwPos, uint32_t dwCount)
{
    if (!::LockFile((void*)m_hFile, dwPos, 0, dwCount, 0))
    {
        bError = TRUE;
    }
}

void CFileIO::UnlockRange(uint32_t dwPos, uint32_t dwCount)
{
    if (!::UnlockFile((void*)m_hFile, dwPos, 0, dwCount, 0))
    {
        bError = TRUE;
    }
}

void CFileIO::SetLength(uint32_t dwNewLen)
{
    Seek((int32_t)dwNewLen, (uint32_t)begin);

    if (!::SetEndOfFile((void*)m_hFile))
    {
        bError = TRUE;
    }
}

uint32_t CFileIO::GetLength()
{
    uint32_t dwLen, dwCur;

    // Seek is a non const operation
    dwCur = ((CFileIO*)this)->Seek(0L, current);
    dwLen = SeekToEnd();

    return dwLen;
}

// CFileIO does not support direct buffering (CMemFile does)
uint32_t CFileIO::GetBufferPtr(uint32_t nCommand, uint32_t /*nCount*/,
    void** /*ppBufStart*/, void** /*ppBufMax*/)
{
    nCommand;   // not used in retail build

    return 0;   // no support
}

int32_t PASCAL CFileIO::Rename(const char* lpszOldName, const char* lpszNewName)
{
    if (!::MoveFile((char*)lpszOldName, (char*)lpszNewName))
    {
        return FALSE;
    }

    return TRUE;
}

int32_t PASCAL CFileIO::Remove(const char* lpszFileName)
{
    if (!::DeleteFile((char*)lpszFileName))
    {
        return FALSE;
    }

    return TRUE;
}

/////////////////////////////////////////////////////////////////////

uint32_t CFileIO::ReadHuge(void* lpBuffer, uint32_t dwCount)
    { return (uint32_t)Read(lpBuffer, (uint32_t)dwCount); }
void CFileIO::WriteHuge(const void* lpBuffer, uint32_t dwCount)
    { Write(lpBuffer, (uint32_t)dwCount); }

uint32_t CFileIO::SeekToEnd()
    { return Seek(0, CFileIO::end); }
void CFileIO::SeekToBegin()
    { Seek(0, CFileIO::begin); }
