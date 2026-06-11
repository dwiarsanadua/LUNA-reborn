// IndexGenerator.h: interface for the CIndexGenerator class.
// Lastest update : 2002.12.29. by taiyo
//////////////////////////////////////////////////////////////////////

#ifndef __CINDEXGENERATOR_H__
#define __CINDEXGENERATOR_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <platform.h>

//////////////////////////////////////////////////////////////////////
// ±âº» ½ÃÀÛ ÀÎµ¦½º´Â 1ÀÌ´Ù.
// index°¡ 0ÀÌ ³Ñ¾î ¿À¸é ¿¡·¯!
//////////////////////////////////////////////////////////////////////
class CIndexGenerator  
{
public:
	CIndexGenerator();
	virtual ~CIndexGenerator();

	void	Init(uint32_t dwMaxIndexNum, uint32_t startIdx=1);
	void	Release();
	uint32_t	GenerateIndex();
	int32_t	ReleaseIndex(uint32_t dwIndex);
private:
	uint32_t * m_pIndexTable;
	uint32_t front;
	uint32_t rear;
	uint32_t m_dwMaxIndexNum;
	uint32_t m_dwStartIndex;
};

#endif // __CINDEXGENERATOR_H__
