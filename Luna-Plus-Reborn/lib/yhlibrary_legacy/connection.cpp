#include "stdafx.h"
#include "connection.h"
#include "define.h"

///////////////////////////////////////////////////////////////////////////
#if defined(_DEBUG) && defined(_MSC_VER)
#include <crtdbg.h>
	#define _DEBUG_NEW new( _NORMAL_BLOCK,__FILE__,__LINE__)
	#define new _DEBUG_NEW 
#else
	#define _DEBUG_NEW 
#endif
///////////////////////////////////////////////////////////////////////////

CConnection::CConnection(SOCKET s,uint32_t size)
{
	m_dwStatus = CONNECTION_STATUS_STANDBY;
	m_pSendBuffer = NULL;
	m_pRecvBuffer = NULL;
	m_socket = s;
	m_dwMaxBufferSize = size;
	m_dwSendBufferPosition = 0;
	m_dwRecvBufferPosition = 0;
	m_pSendBuffer = NULL;
	m_pRecvBuffer = NULL;
	
	m_pSendBuffer = new char[m_dwMaxBufferSize];
	m_pRecvBuffer = new char[m_dwMaxBufferSize];
	
	memset(m_pSendBuffer,0,m_dwMaxBufferSize);
	memset(m_pRecvBuffer,0,m_dwMaxBufferSize);
}

bool CConnection::Send(char* msg,uint32_t length)
{
	if (length >= MAX_PACKET_SIZE)
	{
		return false;
	}
	if (m_dwRecvBufferPosition + length >= m_dwMaxBufferSize)
	{
		SetConnectionStatus(CONNECTION_STATUS_CLOSED_FROM_LOCAL);
		closesocket(m_socket);
		{
		//	MessageBox( NULL, "�������� ������ ������ϴ�","����",MB_OK);	
		}
	    
		return false;
	}
	
	*(uint16_t*)(m_pSendBuffer+m_dwSendBufferPosition) = (uint16_t)length;
	m_dwSendBufferPosition += 2;
	memcpy(m_pSendBuffer+m_dwSendBufferPosition,msg,length);
	m_dwSendBufferPosition += length;

	return SendFromBuffer();
}

bool CConnection::SendFromBuffer()
{	
	if (m_dwSendBufferPosition <= 0)
		return false;

	int result;

	while (m_dwSendBufferPosition>0)
	{
		result = send(m_socket,m_pSendBuffer,m_dwSendBufferPosition,NULL);

		if (result == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				SetConnectionStatus(CONNECTION_STATUS_BLOCKED);
				return true;
			}	
			else
			{
				SetConnectionStatus(CONNECTION_STATUS_CLOSED_FROM_LOCAL);
				closesocket(m_socket);
				return false;
			}
		}
		m_dwSendBufferPosition -= result;
		memcpy(m_pSendBuffer,m_pSendBuffer+result,m_dwSendBufferPosition);
	}
	return true;
}

bool CConnection::OnRead()
{
	int result;

	if (m_dwMaxBufferSize-m_dwRecvBufferPosition <= 0)
	{
		SetConnectionStatus(CONNECTION_STATUS_CLOSED_FROM_LOCAL);
		closesocket(m_socket);
		return false;
	}

	result = recv(m_socket,m_pRecvBuffer+m_dwRecvBufferPosition,m_dwMaxBufferSize-m_dwRecvBufferPosition,NULL);

	if(result == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			SetConnectionStatus(CONNECTION_STATUS_BLOCKED);
			return true;
		}
		else
		{
			SetConnectionStatus(CONNECTION_STATUS_CLOSED_FROM_LOCAL);
			closesocket(m_socket);
			return false;
		}
	}
	m_dwRecvBufferPosition += result;
	
	return true;
}



CConnection::~CConnection()
{
	if (m_pSendBuffer)
	{
		delete [] m_pSendBuffer;
		m_pSendBuffer = NULL;
	}
	if (m_pRecvBuffer)
	{
		delete [] m_pRecvBuffer;
		m_pRecvBuffer = NULL;
	}
	if (GetConnectionStatus() != CONNECTION_STATUS_CLOSED_FROM_LOCAL)
		closesocket(m_socket);

}

bool CConnection::DispatchMsg(char** ppMsg,uint32_t* pdwLen)
{
	if (m_dwRecvBufferPosition < DEFAULT_MESSAGE_LENGTH)
		return false;
	
	*pdwLen = *(uint16_t*)m_pRecvBuffer;
	
	if (m_dwRecvBufferPosition < *pdwLen + DEFAULT_HEADER_SIZE)
		return false;
	
	*ppMsg = m_pRecvBuffer+DEFAULT_HEADER_SIZE;
	
	return true;
}

void CConnection::ShiftRecv(uint32_t dwLen)
{
	m_dwRecvBufferPosition = m_dwRecvBufferPosition - dwLen - 2;
	memcpy(m_pRecvBuffer,m_pRecvBuffer+dwLen+2,m_dwRecvBufferPosition);
}
