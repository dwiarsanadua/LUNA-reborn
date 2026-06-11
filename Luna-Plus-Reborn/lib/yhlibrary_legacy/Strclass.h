#ifndef __STRING_CORE_INCLUDE__
#define __STRING_CORE_INCLUDE__

#include <platform.h>

class CStrClass
{
public:
// Constructors
	CStrClass();
	CStrClass(const CStrClass& stringSrc);
	CStrClass(char ch, int nRepeat = 1);
	CStrClass(const char* lpsz);
	CStrClass(LPCWSTR lpsz);
	CStrClass(const char* lpch, int nLength);
	CStrClass(const unsigned char* psz);

// Attributes & Operations
	// as an array of characters
	int GetLength() const;
	int32_t IsEmpty() const;
	void Empty();						// free up the data

	char GetAt(int nIndex) const;		// 0 based
	char operator[](int nIndex) const; // same as GetAt
	void SetAt(int nIndex, char ch);
	operator const char*() const;			// as a C string

	// overloaded assignment
	const CStrClass& operator=(const CStrClass& stringSrc);
	const CStrClass& operator=(char ch);
#ifdef _UNICODE
	const CStrClass& operator=(char ch);
#endif
	const CStrClass& operator=(const char* lpsz);
	const CStrClass& operator=(LPCWSTR lpsz);
	const CStrClass& operator=(const unsigned char* psz);

	// string concatenation
	const CStrClass& operator+=(const CStrClass& string);
	const CStrClass& operator+=(char ch);
#ifdef _UNICODE
	const CStrClass& operator+=(char ch);
#endif
	const CStrClass& operator+=(const char* lpsz);

	friend CStrClass __cdecl operator+(const CStrClass& string1,const CStrClass& string2);
	friend CStrClass __cdecl operator+(const CStrClass& string, char ch);
	friend CStrClass __cdecl operator+(char ch, const CStrClass& string);
#ifdef _UNICODE
	friend CStrClass __cdecl operator+(const CStrClass& string, char ch);
	friend CStrClass __cdecl operator+(char ch, const CStrClass& string);
#endif
	friend CStrClass __cdecl operator+(const CStrClass& string, const char* lpsz);
	friend CStrClass __cdecl operator+(const char* lpsz, const CStrClass& string);

	// string comparison
	int Compare(const char* lpsz) const;		 // straight character
	int CompareNoCase(const char* lpsz) const;	 // ignore case
	int Collate(const char* lpsz) const;		 // NLS aware

	// simple sub-string extraction
	CStrClass Mid(int nFirst, int nCount) const;
	CStrClass Mid(int nFirst) const;
	CStrClass Left(int nCount) const;
	CStrClass Right(int nCount) const;

	CStrClass SpanIncluding(const char* lpszCharSet) const;
	CStrClass SpanExcluding(const char* lpszCharSet) const;

	// upper/lower/reverse conversion
	void MakeUpper();
	void MakeLower();
	void MakeReverse();

	// searching (return starting index, or -1 if not found)
	// look for a single character match
	int Find(char ch) const;				// like "C" strchr
	int ReverseFind(char ch) const;
	int FindOneOf(const char* lpszCharSet) const;

	// look for a specific sub-string
	int Find(const char* lpszSub) const;		// like "C" strstr

	// simple formatting
	void __cdecl Format(const char* lpszFormat, ...);

	// Windows support
	int32_t LoadString(uint32_t nID);			// load from string resource
										// 255 chars max
#ifndef _UNICODE
	// ANSI <-> OEM support (convert string in place)
	void AnsiToOem();
	void OemToAnsi();
#endif

#ifndef _AFX_NO_BSTR_SUPPORT
	// OLE 2.0 BSTR support (use for OLE automation)
	BSTR AllocSysString();
	BSTR SetSysString(BSTR* pbstr);
#endif

	// Access to string implementation buffer as "C" character array
	char* GetBuffer(int nMinBufLength);
	void ReleaseBuffer(int nNewLength = -1);
	char* GetBufferSetLength(int nNewLength);
	void FreeExtra();

// Implementation
public:
	~CStrClass();
	int GetAllocLength() const;

protected:
	// lengths/sizes in characters
	//	(note: an extra character is always allocated)
	char* m_pchData;			// actual string (zero terminated)
	int m_nDataLength;			// does not include terminating 0
	int m_nAllocLength; 		// does not include terminating 0

	// implementation helpers
	void Init();
	void AllocCopy(CStrClass& dest, int nCopyLen, int nCopyIndex, int nExtraLen) const;
	void AllocBuffer(int nLen);
	void AssignCopy(int nSrcLen, const char* lpszSrcData);
	void ConcatCopy(int nSrc1Len, const char* lpszSrc1Data, int nSrc2Len, const char* lpszSrc2Data);
	void ConcatInPlace(int nSrcLen, const char* lpszSrcData);
	static void SafeDelete(char* lpch);
	static int SafeStrlen(const char* lpsz);

};

// Compare helpers
int32_t __cdecl operator==(const CStrClass& s1, const CStrClass& s2);
int32_t __cdecl operator==(const CStrClass& s1, const char* s2);
int32_t __cdecl operator==(const char* s1, const CStrClass& s2);
int32_t __cdecl operator!=(const CStrClass& s1, const CStrClass& s2);
int32_t __cdecl operator!=(const CStrClass& s1, const char* s2);
int32_t __cdecl operator!=(const char* s1, const CStrClass& s2);
int32_t __cdecl operator<(const CStrClass& s1, const CStrClass& s2);
int32_t __cdecl operator<(const CStrClass& s1, const char* s2);
int32_t __cdecl operator<(const char* s1, const CStrClass& s2);
int32_t __cdecl operator>(const CStrClass& s1, const CStrClass& s2);
int32_t __cdecl operator>(const CStrClass& s1, const char* s2);
int32_t __cdecl operator>(const char* s1, const CStrClass& s2);
int32_t __cdecl operator<=(const CStrClass& s1, const CStrClass& s2);
int32_t __cdecl operator<=(const CStrClass& s1, const char* s2);
int32_t __cdecl operator<=(const char* s1, const CStrClass& s2);
int32_t __cdecl operator>=(const CStrClass& s1, const CStrClass& s2);
int32_t __cdecl operator>=(const CStrClass& s1, const char* s2);
int32_t __cdecl operator>=(const char* s1, const CStrClass& s2);

// conversion helpers
int _wcstombsz_(char* mbstr, const wchar_t* wcstr, size_t count);
int _mbstowcsz_(wchar_t* wcstr, const char* mbstr, size_t count);

// Globals
extern char jazzidChNil;
extern const CStrClass jazzidEmptyString;


#endif
