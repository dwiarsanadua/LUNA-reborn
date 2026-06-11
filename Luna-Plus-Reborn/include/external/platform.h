#pragma once

#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

// ─── Platform Detection ─────────────────────────────────
#if defined(_WIN32) || defined(_WIN64)
    #define LUNA_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
    #define LUNA_PLATFORM_MACOS 1
#elif defined(__linux__)
    #define LUNA_PLATFORM_LINUX 1
#endif


// ─── Basic Types (replacement for Windows typedefs) ─────
using int8    = int8_t;
using int16   = int16_t;
using int32   = int32_t;
using int64   = int64_t;
using __int32 = int32_t;
using __int64 = int64_t;
using uint8   = uint8_t;
using uint16  = uint16_t;
using uint32  = uint32_t;
using uint64  = uint64_t;

using BOOL    = int32_t;
using DOUBLE  = double;
using bool32  = int32_t;
using BYTE    = uint8_t;
using WORD    = uint16_t;
using USHORT  = unsigned short;
using __time64_t = time_t;
using DWORD   = uint32_t;
using LONG    = int32_t;
using ULONG   = uint32_t;
using LONGLONG  = int64_t;
using ULONGLONG = uint64_t;
using DWORD_PTR = uintptr_t;
using DWORDEX   = uint64_t;
using EXPTYPE   = uint64_t;
union LARGE_INTEGER {
    struct { DWORD LowPart; LONG HighPart; };
    long long QuadPart;
};
using PLARGE_INTEGER = LARGE_INTEGER*;

using UINT    = unsigned int;
#define MAXULONG_PTR 0xffffffffUL
using CHAR    = char;
using WCHAR   = wchar_t;
using LPWSTR  = wchar_t*;
using LPCWSTR = const wchar_t*;
using BSTR    = wchar_t*;

using HANDLE  = void*;
using HMODULE = void*;
using HINSTANCE = void*;
using HWND    = void*;
using HDC     = void*;
using HKEY    = void*;

using LPVOID  = void*;
using PVOID   = void*;
#define IN
#define OUT
using LPCVOID = const void*;
using LPTSTR  = char*;
using LPCTSTR = const char*;
using PCHAR  = char*;
using LPSTR  = char*;
using LPCSTR = const char*;
using LPTCH   = char*;
using PTCHAR  = char*;

using WPARAM  = uintptr_t;
using LPARAM  = intptr_t;
using LRESULT = intptr_t;

using HRESULT = int32_t;

// Socket (POSIX)
using SOCKET  = int;
using sockaddr_in = struct sockaddr_in;

#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SD_BOTH 2
#define WSAEWOULDBLOCK EWOULDBLOCK

inline int closesocket(SOCKET s) { return close(s); }

// ─── Constants ──────────────────────────────────────────
#ifndef TRUE
    #define TRUE  1
#endif
#ifndef FALSE
    #define FALSE 0
#endif
#ifndef NULL
    #define NULL  nullptr
#endif
#ifndef MAX_PATH
    #define MAX_PATH 4096
#endif
#ifndef _MAX_PATH
    #define _MAX_PATH 4096
#endif

// ─── String ─────────────────────────────────────────────
using tchar   = char;
using TCHAR   = char;
#define _T(x)  x
#define _TEXT(x) x
#define TEXT(x)  x


// ─── COM Compatibility (minimal stubs) ──────────────────
#define interface struct
#define STDMETHODIMP       int32_t
#define STDMETHODIMP_(type) type
#define STDMETHODCALLTYPE
#define WINAPI
#define CALLBACK
#define PASCAL
#define PPVOID void**

#ifndef IUnknown
struct IUnknown { virtual ~IUnknown() = default; };
#define IUnknown IUnknown
#endif
using REFIID = const int&;
#define IID_IX 0
#define S_OK        0
#define S_FALSE     1
#define E_NOTIMPL   0x80004001L
#define E_FAIL      0x80004005L
#define E_OUTOFMEMORY 0x8007000EL

#ifndef MAX_PATH
#define MAX_PATH    260
#endif

#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD)(b) & 0xffff))) << 16))
#define MAKELPARAM(l, h)    ((LPARAM)MAKELONG(l, h))

inline __time64_t _time64(__time64_t* t) { time_t result = time(nullptr); if (t) *t = result; return result; }
inline struct tm* _localtime64(const __time64_t* t) { return localtime(t); }
inline __time64_t _mktime64(struct tm* t) { return mktime(t); }
inline long long _atoi64(const char* s) { return atoll(s); }

#ifndef _WIN32
    #define __stdcall
    #define __cdecl
    #define __fastcall
    #define __declspec(x)
    #define dllexport
    #define dllimport
    #pragma clang diagnostic ignored "-Wunknown-pragmas"
    #pragma clang diagnostic ignored "-Wpragma-pack"
#endif

#define DECLARE_DYNAMIC(class_name)

// SAFE_RELEASE: call delete instead of COM Release
template<typename T>
inline void SAFE_RELEASE(T*& p) {
    if (p) { delete p; p = nullptr; }
}
#define SAFE_DELETE(p)    do { delete (p); (p) = nullptr; } while(0)
#define SAFE_DELETE_ARRAY(p) do { delete[] (p); (p) = nullptr; } while(0)


// ─── Time ───────────────────────────────────────────────
inline uint32 GetTickCount() {
    using namespace std::chrono;
    return static_cast<uint32>(duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count());
}

inline uint64 GetTickCount64() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

struct SYSTEMTIME {
    uint16 wYear;
    uint16 wMonth;
    uint16 wDayOfWeek;
    uint16 wDay;
    uint16 wHour;
    uint16 wMinute;
    uint16 wSecond;
    uint16 wMilliseconds;
};

inline void GetLocalTime(SYSTEMTIME* st) {
    auto now = std::chrono::system_clock::now();
    auto tt = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    struct tm result;
    localtime_r(&tt, &result);
    st->wYear = result.tm_year + 1900;
    st->wMonth = result.tm_mon + 1;
    st->wDayOfWeek = result.tm_wday;
    st->wDay = result.tm_mday;
    st->wHour = result.tm_hour;
    st->wMinute = result.tm_min;
    st->wSecond = result.tm_sec;
    st->wMilliseconds = static_cast<uint16>(ms.count());
}

using FILETIME = uint64_t;  // 100-ns intervals since 1601-01-01

inline void GetSystemTimeAsFileTime(FILETIME* ft) {
    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto hundred_ns = std::chrono::duration_cast<std::chrono::duration<int64_t, std::ratio<1, 10000000>>>(duration);
    // Convert from Unix epoch (1970) to Windows epoch (1601): 11644473600 seconds
    constexpr int64_t EPOCH_DIFFERENCE = 11644473600LL * 10000000;
    *ft = static_cast<uint64_t>(hundred_ns.count() + EPOCH_DIFFERENCE);
}


// ─── Debug ──────────────────────────────────────────────
inline void OutputDebugString(const char* str) {
    fprintf(stderr, "%s", str);
}

inline void OutputDebugStringA(const char* str) {
    fprintf(stderr, "%s", str);
}

#define TRACE(...)


// ─── Threading ──────────────────────────────────────────
using CRITICAL_SECTION = std::mutex;
inline void InitializeCriticalSection(std::mutex* m) {}
inline void InitializeCriticalSectionEx(std::mutex* m, DWORD, DWORD) {}
inline void EnterCriticalSection(std::mutex* m) { m->lock(); }
inline void LeaveCriticalSection(std::mutex* m) { m->unlock(); }
inline void DeleteCriticalSection(std::mutex* m) {}

inline DWORD WaitForSingleObject(HANDLE, DWORD) { return 0; }
inline DWORD WaitForMultipleObjects(DWORD, HANDLE*, BOOL, DWORD) { return 0; }
// ─── Memory ─────────────────────────────────────────────
inline void ZeroMemory(void* ptr, size_t len) {
    memset(ptr, 0, len);
}

inline void RtlZeroMemory(void* ptr, size_t len) {
    memset(ptr, 0, len);
}

inline void* HeapAlloc(void*, DWORD, size_t size) { return malloc(size); }
inline void HeapFree(void*, DWORD, void* ptr) { free(ptr); }


// ─── Safe strings ───────────────────────────────────────
inline int wsprintf(char* buf, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(buf, MAX_PATH, fmt, args);
    va_end(args);
    return result;
}

inline int wsprintfA(char* buf, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(buf, MAX_PATH, fmt, args);
    va_end(args);
    return result;
}

inline int _stricmp(const char* a, const char* b) {
    return strcasecmp(a, b);
}

inline int _strnicmp(const char* a, const char* b, size_t n) {
    return strncasecmp(a, b, n);
}

inline int _vsnprintf(char* buf, size_t n, const char* fmt, va_list args) {
    return vsnprintf(buf, n, fmt, args);
}

#ifndef _sntprintf
#define _sntprintf _snprintf
#endif
#ifndef strcmpi
#define strcmpi _stricmp
#endif
#ifndef strnicmp
#define _strnicmp _strnicmp
#endif

inline int _snprintf(char* buf, size_t n, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int result = vsnprintf(buf, n, fmt, args);
    va_end(args);
    return result;
}

inline void lstrcpy(char* dst, const char* src) {
    strcpy(dst, src);
}

inline void lstrcat(char* dst, const char* src) {
    strcat(dst, src);
}

inline int lstrlen(const char* s) {
    return static_cast<int>(strlen(s));
}

inline char* _strlwr(char* s) {
    for (char* p = s; *p; ++p) *p = static_cast<char>(tolower(static_cast<unsigned char>(*p)));
    return s;
}

inline char* strlwr(char* s) { return _strlwr(s); }

inline char* _strupr(char* s) {
    for (char* p = s; *p; ++p) *p = static_cast<char>(toupper(static_cast<unsigned char>(*p)));
    return s;
}

inline char* strupr(char* s) { return _strupr(s); }

// TCHAR string functions
inline size_t _tcslen(const char* s) { return strlen(s); }
inline char* _tcschr(char* s, int c) { return strchr(s, c); }
inline const char* _tcschr(const char* s, int c) { return strchr(s, c); }
inline char* _tcspbrk(char* s, const char* accept) { return strpbrk(s, accept); }
inline const char* _tcspbrk(const char* s, const char* accept) { return strpbrk(s, accept); }
inline char* _tcsrchr(char* s, int c) { return strrchr(s, c); }
inline int _tcsncmp(const char* a, const char* b, size_t n) { return strncmp(a, b, n); }
inline int _tcsicmp(const char* a, const char* b) { return strcasecmp(a, b); }
inline int _tcsnicmp(const char* a, const char* b, size_t n) { return strncasecmp(a, b, n); }
inline size_t _tcsspn(const char* s, const char* accept) { return strspn(s, accept); }
inline size_t _tcscspn(const char* s, const char* reject) { return strcspn(s, reject); }
inline char* _tcsstr(char* haystack, const char* needle) { return strstr(haystack, needle); }
inline const char* _tcsstr(const char* haystack, const char* needle) { return strstr(haystack, needle); }
inline char* _tcsinc(const char* p) { return const_cast<char*>(p + 1); }
#define _tcsdec(start, current) ((current) > (start) ? (current) - 1 : nullptr)
#define _tclen(p) 1
inline int _ttoi(const char* s) { return atoi(s); }
inline float _tstof(const char* s) { return strtof(s, nullptr); }
inline char* _tcstok(char* s, const char* delim) { return strtok(s, delim); }

#define _stprintf sprintf
#define _istdigit(c) isdigit(static_cast<unsigned char>(c))
#define _tcscoll strcoll
inline char* _tcsrev(char* s) { 
    size_t len = strlen(s);
    for (size_t i = 0; i < len / 2; i++) {
        char t = s[i]; s[i] = s[len - 1 - i]; s[len - 1 - i] = t;
    }
    return s;
}
inline int lstrlenA(const char* s) { return static_cast<int>(strlen(s)); }

// min/max macros intentionally omitted — use std::min/std::max from <algorithm>

inline char CharUpper(char c) { return static_cast<char>(toupper(static_cast<unsigned char>(c))); }
inline char CharLower(char c) { return static_cast<char>(tolower(static_cast<unsigned char>(c))); }
inline void CharUpper(char* s) { while (*s) { *s = CharUpper(*s); s++; } }
inline void CharLower(char* s) { while (*s) { *s = CharLower(*s); s++; } }
inline void AnsiToOem(const char*, char*) { /* stub */ }
inline void OemToAnsi(const char*, char*) { /* stub */ }
inline int _tcscmp(const char* a, const char* b) { return strcmp(a, b); }
inline char* _tcscpy(char* d, const char* s) { return strcpy(d, s); }
inline char* _tcscat(char* d, const char* s) { return strcat(d, s); }
inline char* _tcsncpy(char* d, const char* s, size_t n) { return strncpy(d, s, n); }
inline char* _tcsdup(const char* s) { return strdup(s); }

// memcpy_s → memcpy (size is assumed valid)
inline int memcpy_s(void* dst, size_t, const void* src, size_t n) {
    memcpy(dst, src, n);
    return 0;
}


// ─── Error ──────────────────────────────────────────────
inline DWORD GetLastError() { return errno; }
inline void SetLastError(DWORD e) { errno = static_cast<int>(e); }
inline DWORD WSAGetLastError() { return errno; }


// ─── Atomics ────────────────────────────────────────────
template<typename T>
inline T InterlockedExchange(T* dest, T val) {
    return std::atomic_exchange(dest, val);
}

template<typename T>
inline T InterlockedIncrement(T* dest) {
    return std::atomic_fetch_add(dest, T(1)) + T(1);
}

template<typename T>
inline T InterlockedDecrement(T* dest) {
    return std::atomic_fetch_sub(dest, T(1)) - T(1);
}

template<typename T, typename V>
inline T InterlockedCompareExchange(T* dest, T exchange, T comperand) {
    std::atomic_compare_exchange_strong(dest, &comperand, exchange);
    return comperand;
}


// ─── Sleep ──────────────────────────────────────────────
inline void Sleep(DWORD ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}


// ─── File API Compatibility ─────────────────────────────
inline int _open(const char* path, int flags, int mode = 0) {
    return ::open(path, flags, mode);
}

inline FILE* _wfopen(const wchar_t*, const wchar_t*) { return nullptr; }
inline int _wchdir(const wchar_t*) { return -1; }

#define FILE_SHARE_READ         0x00000001
#define FILE_SHARE_WRITE        0x00000002
#define GENERIC_READ            0x80000000
#define GENERIC_WRITE           0x40000000
#define CREATE_ALWAYS           2
#define OPEN_EXISTING           3
#define OPEN_ALWAYS             4
#define FILE_ATTRIBUTE_NORMAL   0x00000080
#define INVALID_HANDLE_VALUE    ((HANDLE)-1)
#define DUPLICATE_SAME_ACCESS   2

struct SECURITY_ATTRIBUTES {
    DWORD nLength;
    void* lpSecurityDescriptor;
    BOOL bInheritHandle;
};

inline HANDLE CreateFile(const char* lpFileName, DWORD dwDesiredAccess,
                         DWORD dwShareMode, SECURITY_ATTRIBUTES* lpSecurityAttributes,
                         DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
                         HANDLE hTemplateFile) {
    // TODO: PORT - real file implementation
    return INVALID_HANDLE_VALUE;
}

inline BOOL DuplicateHandle(HANDLE, HANDLE, HANDLE, HANDLE*, DWORD, BOOL, DWORD) {
    return FALSE;
}

inline HANDLE GetCurrentProcess() { return (HANDLE)1; }

inline DWORD GetFileSize(HANDLE, DWORD*) { return 0; }

inline BOOL ReadFile(HANDLE, void*, DWORD, DWORD*, void*) { return FALSE; }

inline BOOL WriteFile(HANDLE, const void*, DWORD, DWORD*, void*) { return FALSE; }

inline BOOL CloseHandle(HANDLE) { return TRUE; }

inline BOOL SetFilePointer(HANDLE, LONG, LONG*, DWORD) { return FALSE; }

inline DWORD SetFilePointerEx(HANDLE, LARGE_INTEGER, PLARGE_INTEGER, DWORD) { return 0; }

inline BOOL GetFileInformationByHandle(HANDLE, void*) { return FALSE; }

using HFILE = int;
#define FILE_CURRENT 1
#define FILE_BEGIN 0
#define FILE_END 2

inline DWORD timeGetTime() {
    return GetTickCount();
}

inline BOOL FlushFileBuffers(HANDLE) { return TRUE; }
inline BOOL LockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) { return TRUE; }
inline BOOL UnlockFile(HANDLE, DWORD, DWORD, DWORD, DWORD) { return TRUE; }
inline BOOL SetEndOfFile(HANDLE) { return TRUE; }
inline BOOL MoveFile(const char*, const char*) { return TRUE; }
inline BOOL DeleteFile(const char*) { return TRUE; }

// ─── Module / Process ───────────────────────────────────
inline DWORD GetModuleFileNameA(void*, char* buf, DWORD size) {
    return static_cast<DWORD>(strlen(buf));
}

inline DWORD GetCurrentDirectoryA(DWORD, char*) { return 0; }


// ─── STL Compatibility (MSVC extensions) ───────────────
// MSVC used stdext::hash_map / stdext::hash_set before C++11
#include <unordered_map>
#include <unordered_set>
namespace stdext {
    template<typename K, typename V>
    using hash_map = std::unordered_map<K, V>;

    template<typename K>
    using hash_set = std::unordered_set<K>;
}


// ─── RGBA ───────────────────────────────────────────────
inline DWORD RGBA_MAKE(BYTE r, BYTE g, BYTE b, BYTE a) {
    return (static_cast<DWORD>(a) << 24) |
           (static_cast<DWORD>(r) << 16) |
           (static_cast<DWORD>(g) << 8)  |
           static_cast<DWORD>(b);
}


// ─── Windows Message Constants ─────────────────────────
#define MB_OK                   0x00000000L
#define MB_YESNO                0x00000004L
#define MB_ICONINFORMATION      0x00000040L
#define MB_TOPMOST              0x00040000L
#define MB_SYSTEMMODAL          0x00001000L
#define IDOK                    1
#define IDCANCEL                2
#define IDYES                   6
#define IDNO                    7

// ─── MessageBox ─────────────────────────────────────────
#ifndef MB_OK
#define MB_OK 0
#endif
#ifndef MB_OKCANCEL
#define MB_OKCANCEL 1
#endif
#ifndef MB_ICONERROR
#define MB_ICONERROR 16
#endif
#ifndef MB_ICONWARNING
#define MB_ICONWARNING 48
#endif
#ifndef MB_ICONINFORMATION
#define MB_ICONINFORMATION 64
#endif
inline int MessageBox(void*, const char* text, const char* caption, int) {
    fprintf(stderr, "%s: %s\n", caption, text);
    return 1;
}


// ─── Character Conversion ──────────────────────────────
#define CP_ACP 0
#define CP_UTF8 65001

inline int WideCharToMultiByte(UINT CodePage, DWORD dwFlags,
    LPCWSTR lpWideCharStr, int cchWideChar,
    char* lpMultiByteStr, int cbMultiByte,
    const char*, const char*) {
    if (!lpWideCharStr || cchWideChar == 0) return 0;
    size_t converted = 0;
    if (lpMultiByteStr && cbMultiByte > 0) {
        for (int i = 0; i < cchWideChar && i < cbMultiByte - 1; i++) {
            lpMultiByteStr[i] = static_cast<char>(lpWideCharStr[i] & 0xFF);
            converted++;
        }
        lpMultiByteStr[converted] = 0;
    } else {
        converted = wcstombs(nullptr, lpWideCharStr, 0);
        if (converted == (size_t)-1) converted = cchWideChar;
    }
    return static_cast<int>(converted);
}

inline int MultiByteToWideChar(UINT CodePage, DWORD dwFlags,
    const char* lpMultiByteStr, int cbMultiByte,
    wchar_t* lpWideCharStr, int cchWideChar) {
    if (!lpMultiByteStr || cbMultiByte == 0) return 0;
    size_t converted = 0;
    if (lpWideCharStr && cchWideChar > 0) {
        converted = mbstowcs(lpWideCharStr, lpMultiByteStr, cchWideChar - 1);
        lpWideCharStr[converted] = 0;
    } else {
        converted = mbstowcs(nullptr, lpMultiByteStr, 0);
        if (converted == (size_t)-1) converted = cbMultiByte;
    }
    return static_cast<int>(converted);
}

// ─── GDI / System Types ──────────────────────────────────
#include <wingdi.h>
typedef char            CHAR;
typedef short           SHORT;
typedef int             INT;
typedef unsigned int    UINT;
typedef float           FLOAT;
typedef void            VOID;
typedef uint32_t        COLORREF;

struct POINT { int32_t x, y; };
struct POINTS { int16_t x, y; };
struct RECT { int32_t left, top, right, bottom; };
struct SIZE { int32_t cx, cy; };

typedef struct HFONT__* HFONT;
typedef struct HICON__* HICON;
typedef struct HCURSOR__* HCURSOR;
typedef struct HBRUSH__* HBRUSH;
typedef struct HPEN__* HPEN;
typedef struct HBITMAP__* HBITMAP;
typedef struct HRGN__* HRGN;

// ─── BMP File Structures ──────────────────────────────
typedef struct tagBITMAPFILEHEADER {
    WORD  bfType;
    DWORD bfSize;
    WORD  bfReserved1;
    WORD  bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *LPBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG  biWidth;
    LONG  biHeight;
    WORD  biPlanes;
    WORD  biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG  biXPelsPerMeter;
    LONG  biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *LPBITMAPINFOHEADER;

// ─── Win32 Window Message Constants (minimal) ───────────
#define WM_DESTROY      0x0002
#define WM_CLOSE        0x0010
#define WM_QUIT         0x0012
#define WM_SIZE         0x0005
#define WM_KEYDOWN      0x0100
#define WM_KEYUP        0x0101
#define WM_CHAR         0x0102
#define WM_LBUTTONDOWN  0x0201
#define WM_LBUTTONUP    0x0202
#define WM_RBUTTONDOWN  0x0204
#define WM_RBUTTONUP    0x0205
#define WM_MOUSEMOVE    0x0200
#define WM_MOUSEWHEEL   0x020A

// ─── ODBC stubs (old MSVC/ODBC types, replaced by IDatabase) ──
#define STR_LEN 128
using HSTMT    = void*;
using UCHAR    = unsigned char;
using SDWORD   = int32_t;
using SQLHDBC  = void*;
using SWORD    = int16_t;
using SFWORD   = int32_t;
using SQLHENV  = void*;
using SQLHSTMT = void*;
using RETCODE  = int16_t;
