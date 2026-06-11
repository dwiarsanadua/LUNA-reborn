/*===========================================================================
 *
 *	Copyright (C) 1997 Hicom Corporation.Ltd. All Rights Reserved.
 *
 *
 *	File   : strclass.cpp
 *	Content: String routine
 *	Program: Lee Jon suk
 *
 ****************************************************************************/
#include "stdafx.h"
//#include <windows.h>
#include <atlbase.h>
//#include <limits.h>
//#include <tchar.h>
#include <algorithm>
#include "stdio.h"
#include "strclass.h"

/////////////////////////////////////////////////////////////////////////////
// static class data, special inlines

// For an empty string, m_???Data will point here
// (note: avoids a lot of NULL pointer tests when we call standard
//	C runtime libraries
char jazzidChNil = '\0';

// for creating empty key strings
const CStrClass jazzidEmptyString;

void CStrClass::Init()
{
	m_nDataLength = m_nAllocLength = 0;
	m_pchData = (char*)&jazzidChNil;
}

// declared static
void CStrClass::SafeDelete(char* lpch)
{
	if (lpch != (char*)&jazzidChNil)
		delete[] lpch;
}

//////////////////////////////////////////////////////////////////////////////
// Construction/Destruction

CStrClass::CStrClass()
{
	Init();
}

CStrClass::CStrClass(const CStrClass& stringSrc)
{
	// if constructing a CStrClass from another CStrClass, we make a copy of the
	// original string data to enforce value semantics (i.e. each string
	// gets a copy of its own

	stringSrc.AllocCopy(*this, stringSrc.m_nDataLength, 0, 0);
}

void CStrClass::AllocBuffer(int nLen)
 // always allocate one extra character for '\0' termination
 // assumes [optimistically] that data length will equal allocation length
{
	if (nLen == 0)
	{
		Init();
	}
	else
	{
		m_pchData = new char[nLen+1];		 // may throw an exception
		m_pchData[nLen] = '\0';
		m_nDataLength = nLen;
		m_nAllocLength = nLen;
	}
}

void CStrClass::Empty()
{
	SafeDelete(m_pchData);
	Init();
}

CStrClass::~CStrClass()
 //  free any attached data
{
	SafeDelete(m_pchData);
}

//////////////////////////////////////////////////////////////////////////////
// Helpers for the rest of the implementation

int SafeStrlen(const char* lpsz)
{
	return (lpsz == NULL) ? NULL : _tcslen(lpsz);
}

int CStrClass::GetLength() const
{
	return m_nDataLength;
}

void CStrClass::AllocCopy(CStrClass& dest, int nCopyLen, int nCopyIndex,
	 int nExtraLen) const
{
	// will clone the data attached to this string
	// allocating 'nExtraLen' characters
	// Places results in uninitialized string 'dest'
	// Will copy the part or all of original data to start of new string

	int nNewLen = nCopyLen + nExtraLen;

	if (nNewLen == 0)
	{
		dest.Init();
	}
	else
	{
		dest.AllocBuffer(nNewLen);
		memcpy(dest.m_pchData, &m_pchData[nCopyIndex], nCopyLen*sizeof(char));
	}
}

//////////////////////////////////////////////////////////////////////////////
// More sophisticated construction

CStrClass::CStrClass(const char* lpsz)
{
	int nLen;
	if ((nLen = SafeStrlen(lpsz)) == 0)
		Init();
	else
	{
		AllocBuffer(nLen);
		memcpy(m_pchData, lpsz, nLen*sizeof(char));
	}
}

/////////////////////////////////////////////////////////////////////////////
// Special conversion constructors

#ifdef _UNICODE
CStrClass::CStrClass(const char* lpsz)
{
	int nSrcLen = lpsz != NULL ? lstrlenA(lpsz) : 0;
	if (nSrcLen == 0)
		Init();
	else
	{
		AllocBuffer(nSrcLen);
		_mbstowcsz_(m_pchData, lpsz, nSrcLen+1);
	}
}
#else //_UNICODE
CStrClass::CStrClass(LPCWSTR lpsz)
{
	int nSrcLen = lpsz != NULL ? wcslen(lpsz) : 0;
	if (nSrcLen == 0)
		Init();
	else
	{
		AllocBuffer(nSrcLen);
		_wcstombsz_(m_pchData, lpsz, nSrcLen+1);
	}
}
#endif //!_UNICODE

//////////////////////////////////////////////////////////////////////////////
// Assignment operators
//	All assign a new value to the string
//		(a) first see if the buffer is big enough
//		(b) if enough room, copy on top of old buffer, set size and type
//		(c) otherwise free old string data, and create a new one
//
//	All routines return the new string (but as a 'const CStrClass&' so that
//		assigning it again will cause a copy, eg: s1 = s2 = "hi there".
//

void CStrClass::AssignCopy(int nSrcLen, const char* lpszSrcData)
{
	// check if it will fit
	if (nSrcLen > m_nAllocLength)
	{
		// it won't fit, allocate another one
		Empty();
		AllocBuffer(nSrcLen);
	}
	if (nSrcLen != 0)
		memcpy(m_pchData, lpszSrcData, nSrcLen*sizeof(char));
	m_nDataLength = nSrcLen;
	m_pchData[nSrcLen] = '\0';
}

const CStrClass& CStrClass::operator=(const CStrClass& stringSrc)
{
	AssignCopy(stringSrc.m_nDataLength, stringSrc.m_pchData);
	return *this;
}

const CStrClass& CStrClass::operator=(const char* lpsz)
{
	AssignCopy(SafeStrlen(lpsz), lpsz);
	return *this;
}

/////////////////////////////////////////////////////////////////////////////
// Special conversion assignment

#ifdef _UNICODE
const CStrClass& CStrClass::operator=(const char* lpsz)
{
	int nSrcLen = lpsz != NULL ? lstrlenA(lpsz) : 0;
	// check if it will fit
	if (nSrcLen > m_nAllocLength)
	{
		// it won't fit, allocate another one
		Empty();
		AllocBuffer(nSrcLen);
	}
	if (nSrcLen != 0)
		_mbstowcsz_(m_pchData, lpsz, nSrcLen+1);
	m_nDataLength = nSrcLen;
	m_pchData[nSrcLen] = '\0';
	return *this;
}
#else //!_UNICODE
const CStrClass& CStrClass::operator=(LPCWSTR lpsz)
{
	int nSrcLen = lpsz != NULL ? wcslen(lpsz) : 0;
	// check if it will fit
	if (nSrcLen > m_nAllocLength)
	{
		// it won't fit, allocate another one
		Empty();
		AllocBuffer(nSrcLen);
	}
	if (nSrcLen != 0)
		_wcstombsz_(m_pchData, lpsz, nSrcLen+1);
	m_nDataLength = nSrcLen;
	m_pchData[nSrcLen] = '\0';
	return *this;
}
#endif	//!_UNICODE

//////////////////////////////////////////////////////////////////////////////
// concatenation

// NOTE: "operator+" is done as friend functions for simplicity
//		There are three variants:
//			CStrClass + CStrClass
// and for ? = char, const char*
//			CStrClass + ?
//			? + CStrClass

void CStrClass::ConcatCopy(int nSrc1Len, const char* lpszSrc1Data,
	int nSrc2Len, const char* lpszSrc2Data)
{
  // -- master concatenation routine
  // Concatenate two sources
  // -- assume that 'this' is a new CStrClass object

	int nNewLen = nSrc1Len + nSrc2Len;
	AllocBuffer(nNewLen);
	memcpy(m_pchData, lpszSrc1Data, nSrc1Len*sizeof(char));
	memcpy(&m_pchData[nSrc1Len], lpszSrc2Data, nSrc2Len*sizeof(char));
}

CStrClass __cdecl operator+(const CStrClass& string1, const CStrClass& string2)
{
	CStrClass s;
	s.ConcatCopy(string1.m_nDataLength, string1.m_pchData,
		string2.m_nDataLength, string2.m_pchData);
	return s;
}

CStrClass __cdecl operator+(const CStrClass& string, const char* lpsz)
{
	CStrClass s;
	s.ConcatCopy(string.m_nDataLength, string.m_pchData, SafeStrlen(lpsz), lpsz);
	return s;
}

CStrClass __cdecl operator+(const char* lpsz, const CStrClass& string)
{
	CStrClass s;
	s.ConcatCopy(SafeStrlen(lpsz), lpsz, string.m_nDataLength, string.m_pchData);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// concatenate in place

void CStrClass::ConcatInPlace(int nSrcLen, const char* lpszSrcData)
{
	//	-- the main routine for += operators

	// if the buffer is too small, or we have a width mis-match, just
	//	 allocate a new buffer (slow but sure)
	if (m_nDataLength + nSrcLen > m_nAllocLength)
	{
		// we have to grow the buffer, use the Concat in place routine
		char* lpszOldData = m_pchData;
		ConcatCopy(m_nDataLength, lpszOldData, nSrcLen, lpszSrcData);
		SafeDelete(lpszOldData);
	}
	else
	{
		// fast concatenation when buffer big enough
		memcpy(&m_pchData[m_nDataLength], lpszSrcData, nSrcLen*sizeof(char));
		m_nDataLength += nSrcLen;
	}
	m_pchData[m_nDataLength] = '\0';
}

const CStrClass& CStrClass::operator+=(const char* lpsz)
{
	ConcatInPlace(SafeStrlen(lpsz), lpsz);
	return *this;
}

const CStrClass& CStrClass::operator+=(char ch)
{
	ConcatInPlace(1, &ch);
	return *this;
}

const CStrClass& CStrClass::operator+=(const CStrClass& string)
{
	ConcatInPlace(string.m_nDataLength, string.m_pchData);
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
// Advanced direct buffer access

char* CStrClass::GetBuffer(int nMinBufLength)
{
	if (nMinBufLength > m_nAllocLength)
	{
		// we have to grow the buffer
		char* lpszOldData = m_pchData;
		int nOldLen = m_nDataLength;		// AllocBuffer will tromp it

		AllocBuffer(nMinBufLength);
		memcpy(m_pchData, lpszOldData, nOldLen*sizeof(char));
		m_nDataLength = nOldLen;
		m_pchData[m_nDataLength] = '\0';	

		SafeDelete(lpszOldData);
	}

	// return a pointer to the character storage for this string
	return m_pchData;
}

void CStrClass::ReleaseBuffer(int nNewLength)
{
	if (nNewLength == -1)
		nNewLength = lstrlen(m_pchData); // zero terminated

	m_nDataLength = nNewLength;
	m_pchData[m_nDataLength] = '\0';
}

char* CStrClass::GetBufferSetLength(int nNewLength)
{

	GetBuffer(nNewLength);
	m_nDataLength = nNewLength;
	m_pchData[m_nDataLength] = '\0';
	return m_pchData;
}

void CStrClass::FreeExtra()
{
	if (m_nDataLength != m_nAllocLength)
	{
		char* lpszOldData = m_pchData;
		AllocBuffer(m_nDataLength);
		memcpy(m_pchData, lpszOldData, m_nDataLength*sizeof(char));
		SafeDelete(lpszOldData);
	}
}

///////////////////////////////////////////////////////////////////////////////
// Commonly used routines (rarely used routines in STREX.CPP)

int CStrClass::Find(char ch) const
{
	// find first single character
	char* lpsz = _tcschr(m_pchData, ch);

	// return -1 if not found and index otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

int CStrClass::FindOneOf(const char* lpszCharSet) const
{
	char* lpsz = _tcspbrk(m_pchData, lpszCharSet);
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

///////////////////////////////////////////////////////////////////////////////
// CStrClass conversion helpers (these use the current system locale)

int _wcstombsz_(char* mbstr, const wchar_t* wcstr, size_t count)
{
	if (count == 0 && mbstr != NULL)
		return 0;

	int result = ::WideCharToMultiByte(CP_ACP, 0, wcstr, -1,
		mbstr, count, NULL, NULL);
	if (result > 0)
		mbstr[result-1] = 0;
	return result;
}

int _mbstowcsz_(wchar_t* wcstr, const char* mbstr, size_t count)
{
	if (count == 0 && wcstr != NULL)
		return 0;

	int result = ::MultiByteToWideChar(CP_ACP, 0, mbstr, -1,
		wcstr, count);
	if (result > 0)
		wcstr[result-1] = 0;
	return result;
}

//////////////////////////////////////////////////////////////////////////////
// More sophisticated construction

CStrClass::CStrClass(char ch, int nLength)
{
	if (nLength < 1)
	{
		// return empty string if invalid repeat count
		Init();
	}
	else
	{
		AllocBuffer(nLength);
#ifdef _UNICODE
		for (int i = 0; i < nLength; i++)
			m_pchData[i] = ch;
#else
		memset(m_pchData, ch, nLength);
#endif
	}
}

CStrClass::CStrClass(const char* lpch, int nLength)
{
	if (nLength == 0)
		Init();
	else
	{
		AllocBuffer(nLength);
		memcpy(m_pchData, lpch, nLength*sizeof(char));
	}
}

//////////////////////////////////////////////////////////////////////////////
// Assignment operators

const CStrClass& CStrClass::operator=(char ch)
{
	AssignCopy(1, &ch);
	return *this;
}

//////////////////////////////////////////////////////////////////////////////
// less common string expressions

CStrClass __cdecl operator+(const CStrClass& string1, char ch)
{
	CStrClass s;
	s.ConcatCopy(string1.m_nDataLength, string1.m_pchData, 1, &ch);
	return s;
}

CStrClass __cdecl operator+(char ch, const CStrClass& string)
{
	CStrClass s;
	s.ConcatCopy(1, &ch, string.m_nDataLength, string.m_pchData);
	return s;
}

//////////////////////////////////////////////////////////////////////////////
// Very simple sub-string extraction

CStrClass CStrClass::Mid(int nFirst) const
{
	return Mid(nFirst, m_nDataLength - nFirst);
}

CStrClass CStrClass::Mid(int nFirst, int nCount) const
{
	// out-of-bounds requests return sensible things
	if (nFirst + nCount > m_nDataLength)
		nCount = m_nDataLength - nFirst;
	if (nFirst > m_nDataLength)
		nCount = 0;

	CStrClass dest;
	AllocCopy(dest, nCount, nFirst, 0);
	return dest;
}

CStrClass CStrClass::Right(int nCount) const
{

	if (nCount > m_nDataLength)
		nCount = m_nDataLength;

	CStrClass dest;
	AllocCopy(dest, nCount, m_nDataLength-nCount, 0);
	return dest;
}

CStrClass CStrClass::Left(int nCount) const
{

	if (nCount > m_nDataLength)
		nCount = m_nDataLength;

	CStrClass dest;
	AllocCopy(dest, nCount, 0, 0);
	return dest;
}

// strspn equivalent
CStrClass CStrClass::SpanIncluding(const char* lpszCharSet) const
{
	return Left(_tcsspn(m_pchData, lpszCharSet));
}

// strcspn equivalent
CStrClass CStrClass::SpanExcluding(const char* lpszCharSet) const
{
	return Left(_tcscspn(m_pchData, lpszCharSet));
}

//////////////////////////////////////////////////////////////////////////////
// Finding

int CStrClass::ReverseFind(char ch) const
{
	// find last single character
	char* lpsz = _tcsrchr(m_pchData, ch);

	// return -1 if not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

// find a sub-string (like strstr)
int CStrClass::Find(const char* lpszSub) const
{

	// find first matching substring
	char* lpsz = _tcsstr(m_pchData, lpszSub);

	// return -1 for not found, distance from beginning otherwise
	return (lpsz == NULL) ? -1 : (int)(lpsz - m_pchData);
}

/////////////////////////////////////////////////////////////////////////////
// CStrClass formatting

#define FORCE_ANSI		0x10000
#define FORCE_UNICODE	0x20000

// formatting (using wsprintf style formatting)
void __cdecl CStrClass::Format(const char* lpszFormat, ...)
{

	va_list argList;
	va_start(argList, lpszFormat);

	// make a guess at the maximum length of the resulting string
	int nMaxLen = 0;
	for (const char* lpsz = lpszFormat; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
	{
		// handle '%' character, but watch out for '%%'
		if (*lpsz != '%' || *(lpsz = _tcsinc(lpsz)) == '%')
		{
			nMaxLen += _tclen(lpsz);
			continue;
		}

		int nItemLen = 0;

		// handle '%' character with format
		int nWidth = 0;
		for (; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
		{
			// check for valid flags
			if (*lpsz == '#')
				nMaxLen += 2;	// for '0x'
			else if (*lpsz == '*')
				nWidth = *va_arg(argList, int*);
			else if (*lpsz == '-' || *lpsz == '+' || *lpsz == '0' ||
				*lpsz == ' ')
				;
			else // hit non-flag character
				break;
		}
		// get width and skip it
		if (nWidth == 0)
		{
			// width indicated by
			int nWidth = _ttoi(lpsz);
			for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
				;
		}

		int nPrecision = 0;
		if (*lpsz == '.')
		{
			// skip past '.' separator (width.precision)
			lpsz = _tcsinc(lpsz);

			// get precision and skip it
			if (*lpsz == '*')
				nPrecision = *va_arg(argList, int*);
			else
			{
				nPrecision = _ttoi(lpsz);
				for (; *lpsz != '\0' && _istdigit(*lpsz); lpsz = _tcsinc(lpsz))
					;
			}
		}

		// should be on type modifier or specifier
		int nModifier = 0;
		switch (*lpsz)
		{
		// modifiers that affect size
		case 'h':
			nModifier = FORCE_ANSI;
			lpsz = _tcsinc(lpsz);
			break;
		case 'l':
			nModifier = FORCE_UNICODE;
			lpsz = _tcsinc(lpsz);
			break;

		// modifiers that do not affect size
		case 'F':
		case 'N':
		case 'L':
			lpsz = _tcsinc(lpsz);
			break;
		}

		// now should be on specifier
		switch (*lpsz | nModifier)
		{
		// single characters
		case 'c':
		case 'C':
			nItemLen = 2;
			va_arg(argList, char);
			break;
		case 'c'|FORCE_ANSI:
		case 'C'|FORCE_ANSI:
			nItemLen = 2;
			va_arg(argList, char);
			break;
		case 'c'|FORCE_UNICODE:
		case 'C'|FORCE_UNICODE:
			nItemLen = 2;
			va_arg(argList, WCHAR);
			break;

		// strings
		case 's':
		case 'S':
			nItemLen = lstrlen(va_arg(argList, const char*));
			nItemLen = std::max(1, nItemLen);
			break;
		case 's'|FORCE_ANSI:
		case 'S'|FORCE_ANSI:
			nItemLen = lstrlenA(va_arg(argList, const char*));
			nItemLen = std::max(1, nItemLen);
			break;
#ifndef _MAC
		case 's'|FORCE_UNICODE:
		case 'S'|FORCE_UNICODE:
			nItemLen = wcslen(va_arg(argList, LPWSTR));
			nItemLen = std::max(1, nItemLen);
			break;
#endif
		}

		// adjust nItemLen for strings
		if (nItemLen != 0)
		{
			nItemLen = std::max(nItemLen, nWidth);
			if (nPrecision != 0)
				nItemLen = std::min(nItemLen, nPrecision);
		}
		else
		{
			switch (*lpsz)
			{
			// integers
			case 'd':
			case 'i':
			case 'u':
			case 'x':
			case 'X':
			case 'o':
				va_arg(argList, int);
				nItemLen = 32;
				nItemLen = std::max(nItemLen, nWidth+nPrecision);
				break;

			case 'e':
			case 'f':
			case 'g':
			case 'G':
				va_arg(argList, double);
				nItemLen = 32;
				nItemLen = std::max(nItemLen, nWidth+nPrecision);
				break;

			case 'p':
				va_arg(argList, void*);
				nItemLen = 32;
				nItemLen = std::max(nItemLen, nWidth+nPrecision);
				break;

			// no output
			case 'n':
				va_arg(argList, int*);
				break;
			}
		}

		// adjust nMaxLen for output nItemLen
		nMaxLen += nItemLen;
	}
	va_end(argList);

	// finally, set the buffer length and format the string
	va_start(argList, lpszFormat);	// restart the arg list
	vsprintf(GetBuffer(nMaxLen),lpszFormat,argList);
	ReleaseBuffer();
	va_end(argList);
}

///////////////////////////////////////////////////////////////////////////////
// CStrClass support for template collections

void __cdecl ConstructElements(CStrClass* pElements, int nCount)
{
	for (; nCount--; ++pElements)
		memcpy(pElements, &jazzidEmptyString, sizeof(*pElements));
}

void __cdecl DestructElements(CStrClass* pElements, int nCount)
{
	for (; nCount--; ++pElements)
		pElements->Empty();
}

uint32_t __cdecl HashKey(const char* key)
{
	uint32_t nHash = 0;
	while (*key)
		nHash = (nHash<<5) + nHash + *key++;
	return nHash;
}

///////////////////////////////////////////////////////////////////////////////
// CStrClass
 CStrClass::CStrClass(const unsigned char* lpsz)
	{ Init(); *this = (const char*)lpsz; }
 const CStrClass& CStrClass::operator=(const unsigned char* lpsz)
	{ *this = (const char*)lpsz; return *this; }
#ifdef _UNICODE
 const CStrClass& CStrClass::operator+=(char ch)
	{ *this += (char)ch; return *this; }
 const CStrClass& CStrClass::operator=(char ch)
	{ *this = (char)ch; return *this; }
 CStrClass __cdecl operator+(const CStrClass& string, char ch)
	{ return string + (char)ch; }
 CStrClass __cdecl operator+(char ch, const CStrClass& string)
	{ return (char)ch + string; }
#endif

 int CStrClass::GetAllocLength() const
	{ return m_nAllocLength; }
 int32_t CStrClass::IsEmpty() const
	{ return m_nDataLength == 0; }
 CStrClass::operator const char*() const
	{ return (const char*)m_pchData; }
 int CStrClass::SafeStrlen(const char* lpsz)
	{ return (lpsz == NULL) ? NULL : _tcslen(lpsz); }

// CStrClass support (windows specific)
 int CStrClass::Compare(const char* lpsz) const
	{ return _tcscmp(m_pchData, lpsz); }	// MBCS/Unicode aware
 int CStrClass::CompareNoCase(const char* lpsz) const
	{ return _tcsicmp(m_pchData, lpsz); }	// MBCS/Unicode aware
// CStrClass::Collate is often slower than Compare but is MBSC/Unicode
//	aware as well as locale-sensitive with respect to sort order.
 int CStrClass::Collate(const char* lpsz) const
	{ return _tcscoll(m_pchData, lpsz); }	// locale sensitive
 void CStrClass::MakeUpper()
	{ ::CharUpper(m_pchData); }
 void CStrClass::MakeLower()
	{ ::CharLower(m_pchData); }

 void CStrClass::MakeReverse()
	{ _tcsrev(m_pchData); }
 char CStrClass::GetAt(int nIndex) const
	{
		if (nIndex < 0) return 0;
		if (nIndex > m_nDataLength) return 0;

		return m_pchData[nIndex];
	}
 char CStrClass::operator[](int nIndex) const
	{
		// same as GetAt

		if (nIndex < 0) return 0;
		if (nIndex > m_nDataLength) return 0;

		return m_pchData[nIndex];
	}
 void CStrClass::SetAt(int nIndex, char ch)
	{
		if (nIndex < 0) return;
		if (nIndex > m_nDataLength) return;
		if (ch == 0) return;

		m_pchData[nIndex] = ch;
	}
 int32_t __cdecl operator==(const CStrClass& s1, const CStrClass& s2)
	{ return s1.Compare(s2) == 0; }
 int32_t __cdecl operator==(const CStrClass& s1, const char* s2)
	{ return s1.Compare(s2) == 0; }
 int32_t __cdecl operator==(const char* s1, const CStrClass& s2)
	{ return s2.Compare(s1) == 0; }
 int32_t __cdecl operator!=(const CStrClass& s1, const CStrClass& s2)
	{ return s1.Compare(s2) != 0; }
 int32_t __cdecl operator!=(const CStrClass& s1, const char* s2)
	{ return s1.Compare(s2) != 0; }
 int32_t __cdecl operator!=(const char* s1, const CStrClass& s2)
	{ return s2.Compare(s1) != 0; }
 int32_t __cdecl operator<(const CStrClass& s1, const CStrClass& s2)
	{ return s1.Compare(s2) < 0; }
 int32_t __cdecl operator<(const CStrClass& s1, const char* s2)
	{ return s1.Compare(s2) < 0; }
 int32_t __cdecl operator<(const char* s1, const CStrClass& s2)
	{ return s2.Compare(s1) > 0; }
 int32_t __cdecl operator>(const CStrClass& s1, const CStrClass& s2)
	{ return s1.Compare(s2) > 0; }
 int32_t __cdecl operator>(const CStrClass& s1, const char* s2)
	{ return s1.Compare(s2) > 0; }
 int32_t __cdecl operator>(const char* s1, const CStrClass& s2)
	{ return s2.Compare(s1) < 0; }
 int32_t __cdecl operator<=(const CStrClass& s1, const CStrClass& s2)
	{ return s1.Compare(s2) <= 0; }
 int32_t __cdecl operator<=(const CStrClass& s1, const char* s2)
	{ return s1.Compare(s2) <= 0; }
 int32_t __cdecl operator<=(const char* s1, const CStrClass& s2)
	{ return s2.Compare(s1) >= 0; }
 int32_t __cdecl operator>=(const CStrClass& s1, const CStrClass& s2)
	{ return s1.Compare(s2) >= 0; }
 int32_t __cdecl operator>=(const CStrClass& s1, const char* s2)
	{ return s1.Compare(s2) >= 0; }
 int32_t __cdecl operator>=(const char* s1, const CStrClass& s2)
	{ return s2.Compare(s1) <= 0; }

#ifndef _UNICODE
 void CStrClass::AnsiToOem()
	{ ::AnsiToOem(m_pchData, m_pchData); }
 void CStrClass::OemToAnsi()
	{ ::OemToAnsi(m_pchData, m_pchData); }
#endif
