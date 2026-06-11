#pragma once

#include "stdafx.h"

//#pragma pack(push,4)
struct DESC_BASENETWORK
{
	uint16_t		wSockEventWinMsgID;
    void		(*ReceivedMsg)(uint32_t dwInex,char* pMsg,uint32_t dwLen);
	void		(*OnDisconnect)(uint32_t dwInex);
	void		(*OnConnect)(uint32_t dwInex);
};

//#pragma pack(pop)