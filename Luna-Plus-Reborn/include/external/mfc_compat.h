#pragma once
#include <string>
#include <fstream>
#include <exception>
#include <vector>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cctype>
#include <unordered_map>

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif
#ifndef VERIFY
#define VERIFY(x) x
#endif
#ifndef TRACE
#define TRACE
#endif

#define _MAX_DRIVE 3
#define _MAX_DIR 256
#define _MAX_FNAME 256
#define _MAX_EXT 256

// ─── CException → std::exception (defined first) ─────────
class CException : public std::exception {
    int m_error = 0;
    char m_msg[256]{};
public:
    CException(int err = 0) : m_error(err) { snprintf(m_msg, sizeof(m_msg), "CException: %d", err); }
    const char* what() const noexcept override { return m_msg; }
    int GetError() const { return m_error; }
    void Delete() { delete this; }
};

// ─── CFile → std::fstream ──────────────────────────────────
class CFile {
public:
    static constexpr uint64_t hFileNull = (uint64_t)-1;
    enum { modeRead = 1, modeWrite = 2, modeCreate = 4, modeReadWrite = 3,
           typeFile = 2, shareDenyWrite = 32, shareDenyRead = 48,
           begin = 0, current = 1, end = 2 };

    uint64_t m_hFile = hFileNull;

    CFile() = default;
    CFile(const char* path, uint32_t mode) { Open(path, mode, nullptr); }
    virtual ~CFile() { Close(); }

    bool Open(const char* path, uint32_t mode, CFileException* e = nullptr) {
        m_path = path ? path : "";
        auto flags = std::ios::binary;
        if (mode & 1) flags |= std::ios::in;
        if (mode & 2) flags |= std::ios::out;
        if (mode & 4) flags |= std::ios::trunc;
        m_file.open(path ? path : "", flags);
        m_hFile = m_file.is_open() ? 1 : hFileNull;
        if (!m_file.is_open() && e) throw *e;
        return m_file.is_open();
    }
    void Close() { if (m_file.is_open()) { m_file.close(); m_hFile = hFileNull; } }
    void Abort() { Close(); }

    uint64_t GetLength() const {
        if (!m_file.is_open()) return 0;
        auto pos = m_file.tellg();
        const_cast<std::fstream&>(m_file).seekg(0, std::ios::end);
        auto len = (uint64_t)m_file.tellg();
        const_cast<std::fstream&>(m_file).seekg(pos);
        return len;
    }
    void SetLength(uint64_t) {}
    uint64_t GetPosition() const {
        if (!m_file.is_open()) return 0;
        return (uint64_t)m_file.tellg();
    }
    void Flush() { if (m_file.is_open()) m_file.flush(); }
    
    CString GetFilePath() const { return CString(m_path.c_str()); }
    void SetFilePath(const char* path) { m_path = path ? path : ""; }
    
    uint64_t Read(void* buf, uint64_t count) {
        if (!m_file.is_open()) return 0;
        m_file.read((char*)buf, (std::streamsize)count);
        return (uint64_t)m_file.gcount();
    }
    uint64_t Write(const void* buf, uint64_t count) {
        if (!m_file.is_open()) return 0;
        m_file.write((const char*)buf, (std::streamsize)count);
        return count;
    }
    uint64_t Seek(int64_t offset, uint32_t origin) {
        if (!m_file.is_open()) return 0;
        auto way = std::ios::beg;
        if (origin == current) way = std::ios::cur;
        if (origin == end) way = std::ios::end;
        m_file.seekg(offset, way);
        m_file.seekp(offset, way);
        return (uint64_t)m_file.tellg();
    }
    
    static void Remove(const char* path) { std::remove(path); }
    static void Rename(const char* old, const char* nw) { std::rename(old, nw); }

protected:
    mutable std::fstream m_file;
    std::string m_path;
};

typedef CFileException CFileException;

// ─── CString wrapper ─────────────────────────────────────
class CString {
    std::string m_s;
public:
    CString() = default;
    CString(const char* s) : m_s(s ? s : "") {}
    CString(const std::string& s) : m_s(s) {}
    explicit CString(const CString& other, int /*offset*/) : m_s(other.m_s) {}
    
    operator const char*() const { return m_s.c_str(); }
    operator std::string() const { return m_s; }
    
    CString& operator=(const char* s) { m_s = s ? s : ""; return *this; }
    CString& operator=(const std::string& s) { m_s = s; return *this; }
    CString operator+(const char* s) const { return CString(m_s + s); }
    CString operator+(const CString& s) const { return CString(m_s + s.m_s); }
    bool operator==(const char* s) const { return m_s == s; }
    bool operator!=(const char* s) const { return m_s != s; }
    char operator[](int i) const { return (i >= 0 && i < (int)m_s.size()) ? m_s[i] : 0; }
    char& operator[](int i) { return m_s[i]; }
    
    const char* GetString() const { return m_s.c_str(); }
    int GetLength() const { return (int)m_s.length(); }
    bool IsEmpty() const { return m_s.empty(); }
    void Empty() { m_s.clear(); }
    
    void TrimRight(const char* chars = " \t\n\r") {
        auto p = m_s.find_last_not_of(chars);
        if (p != std::string::npos) m_s.erase(p + 1);
    }
    void TrimLeft(const char* chars = " \t\n\r") {
        auto p = m_s.find_first_not_of(chars);
        if (p != std::string::npos) m_s.erase(0, p);
    }
    void MakeUpper() { for (auto& c : m_s) c = (char)toupper(c); }
    void MakeLower() { for (auto& c : m_s) c = (char)tolower(c); }
    
    int Find(char c, int start = 0) const { return (int)m_s.find(c, start); }
    int Find(const char* s, int start = 0) const { return (int)m_s.find(s, start); }
    int ReverseFind(char c) const { return (int)m_s.rfind(c); }
    
    CString Left(int n) const { return CString(m_s.substr(0, n)); }
    CString Right(int n) const { return CString(m_s.substr(m_s.length() - n)); }
    CString Mid(int start, int n = -1) const {
        return (n < 0) ? CString(m_s.substr(start)) : CString(m_s.substr(start, n));
    }
    
    void Append(const char* s) { m_s += s; }
    int Compare(const char* s) const { return m_s.compare(s); }
    int CompareNoCase(const char* s) const {
        auto a = m_s, b = std::string(s ? s : "");
        for (auto& c : a) c = (char)toupper(c);
        for (auto& c : b) c = (char)toupper(c);
        return a.compare(b);
    }
    void Format(const char* fmt, ...) {
        char buf[1024]; va_list ap; va_start(ap, fmt);
        vsnprintf(buf, sizeof(buf), fmt, ap); va_end(ap); m_s = buf;
    }
};

// ─── CObject stub ─────────────────────────────────────────
class CObject {
public:
    virtual ~CObject() = default;
};

// ─── MFC collection types ─────────────────────────────────
template<typename T>
class CArray : public std::vector<T> {};
template<typename T>
class CList : public std::vector<T> {};
template<typename K, typename V>
class CMap : public std::unordered_map<K, V> {};

// ─── MFC runtime macros (stub) ────────────────────────────
#define DECLARE_DYNAMIC(class_name)
#define IMPLEMENT_DYNAMIC(class_name, base_class_name)
#define IMPLEMENT_DYNCREATE(class_name, base_class_name)
#define RUNTIME_CLASS(class_name) ((CObject*)nullptr)
