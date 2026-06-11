#pragma once

#include "StdAfx.h"
#include "define.h"

#define CONNECTION_CLOSED_BIT_MASK	0x00000008

enum CONNECTION_STATUS 
{
	CONNECTION_STATUS_CLOSED_FROM_LOCAL	 =	0x00000009,
	CONNECTION_STATUS_CLOSED_FROM_REMOTE =	0x00000010,
	CONNECTION_STATUS_BLOCKED			 =  1020,
	CONNECTION_STATUS_STANDBY			 =  1000,
};
class CConnection 
{
	CONNECTION_STATUS	m_dwStatus;
	SOCKET				m_socket;
	char*				m_pSendBuffer;
	char*				m_pRecvBuffer;


	uint32_t				m_dwMaxBufferSize;
	uint32_t				m_dwSendBufferPosition;
	uint32_t				m_dwRecvBufferPosition;
public:
	bool				SendFromBuffer();
	uint32_t				GetSendBufferPosition() {return m_dwSendBufferPosition;}
	uint32_t				GetReceiveBufferPosition() {return m_dwRecvBufferPosition;}
	
	void				SetConnectionStatus(CONNECTION_STATUS st) { m_dwStatus = st;}
	CONNECTION_STATUS	GetConnectionStatus() {return m_dwStatus;}
	bool				Send(char* msg,uint32_t length);
	bool				OnRead();
	SOCKET				GetSocket() {return m_socket;}
	char*				GetReceiveBufferPtr() {return m_pRecvBuffer;}
	char*				GetSendBufferPtr() {return m_pSendBuffer;}
	void				Init(SOCKET s,uint32_t size);

	bool				DispatchMsg(char** ppMsg,uint32_t* pdwLen);
	void				ShiftRecv(uint32_t dwLen);

	
	CConnection(SOCKET s,uint32_t size);
	~CConnection();

	CConnection*	m_pNextConnection;
	CConnection*	m_pPrvConnection;
};

typedef CConnection* LPCConnection;


