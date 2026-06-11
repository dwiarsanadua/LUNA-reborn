#pragma once
// Stub: Minimal MFC compatibility for ZipArchive porting
#include <platform.h>
#include <string>
#include <vector>
#include <fstream>

using CString = std::string;
using LPCTSTR = const char*;
using UINT = unsigned int;

class CFile {
public:
    enum { modeRead = 1, modeWrite = 2, modeCreate = 4, modeNoTruncate = 8,
           typeBinary = 16, typeText = 32, shareDenyWrite = 64, shareDenyRead = 128 };
    CFile() : m_hFile(0) {}
    virtual ~CFile() { Close(); }
    virtual BOOL Open(const char* fileName, UINT openFlags);
    virtual void Close();
    virtual UINT Read(void* lpBuf, UINT nCount);
    virtual void Write(const void* lpBuf, UINT nCount);
    virtual LONG Seek(LONG lOff, UINT nFrom);
    virtual ULONGLONG GetPosition() const;
    virtual ULONGLONG GetLength() const;
    void SetLength(ULONGLONG dwNewLen);
    UINT m_hFile;
};

class CException {
public:
    virtual ~CException() {}
    virtual BOOL GetErrorMessage(char* lpszError, UINT nMaxError) { return FALSE; }
};

class CArchive {
public:
    CArchive(CFile* pFile, UINT nMode);
    virtual ~CArchive();
    UINT Read(void* lpBuf, UINT nMax);
    void Write(const void* lpBuf, UINT nMax);
    void Flush();
    void Close();
    BOOL IsLoading() const;
    BOOL IsStoring() const;
};
