#ifndef __FASTFILEIO_INCLUDE__
#define __FASTFILEIO_INCLUDE__

/////////////////////////////////////////////////////////////////////////////
// File - raw unbuffered disk file I/O

#define FILEIO_ERROR (0xffffffff)

class CFileIO
{
public:
// Error boolean
    int32_t bError;
// Flag values
    enum OpenFlags {
        modeRead =          0x0000,
        modeWrite =         0x0001,
        modeReadWrite =     0x0002,
        shareCompat =       0x0000,
        shareExclusive =    0x0010,
        shareDenyWrite =    0x0020,
        shareDenyRead =     0x0030,
        shareDenyNone =     0x0040,
        modeNoInherit =     0x0080,
        modeCreate =        0x1000,
        typeText =          0x4000, // typeText and typeBinary are used in
        typeBinary =   (int)0x8000 // derived classes only
        };

    enum Attribute {
        normal =    0x00,
        readOnly =  0x01,
        hidden =    0x02,
        system =    0x04,
        volume =    0x08,
        directory = 0x10,
        archive =   0x20
        };

    enum SeekPosition { begin = 0x0, current = 0x1, end = 0x2 };

    enum { hFileNull = -1 };

// Constructors
    CFileIO();
    CFileIO(int hFile);
    CFileIO(const char* lpszFileName, uint32_t nOpenFlags);

// Attributes
    uint32_t m_hFile;

    virtual uint32_t GetPosition() const;

// Operations
    virtual int32_t Open(const char* lpszFileName, uint32_t nOpenFlags);

    static int32_t PASCAL Rename(const char* lpszOldName,
                const char* lpszNewName);
    static int32_t PASCAL Remove(const char* lpszFileName);

    uint32_t SeekToEnd();
    void SeekToBegin();

    // backward compatible ReadHuge and WriteHuge
    uint32_t ReadHuge(void* lpBuffer, uint32_t dwCount);
    void WriteHuge(const void* lpBuffer, uint32_t dwCount);

// Overridables
    virtual CFileIO* Duplicate() const;

    virtual int32_t Seek(int32_t lOff, uint32_t nFrom);
    virtual void SetLength(uint32_t dwNewLen);
    virtual uint32_t GetLength();

    virtual uint32_t Read(void* lpBuf, uint32_t nCount);
    virtual void Write(const void* lpBuf, uint32_t nCount);

    virtual void LockRange(uint32_t dwPos, uint32_t dwCount);
    virtual void UnlockRange(uint32_t dwPos, uint32_t dwCount);

    virtual void Abort();
    virtual void Flush();
    virtual void Close();

// Implementation
public:
    virtual ~CFileIO();
    enum BufferCommand { bufferRead, bufferWrite, bufferCommit, bufferCheck };
    virtual uint32_t GetBufferPtr(uint32_t nCommand, uint32_t nCount = 0,
        void** ppBufStart = NULL, void** ppBufMax = NULL);

protected:
    int32_t m_bCloseOnDelete;
};

#endif
