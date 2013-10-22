#include "stdafx.h"
#include "Config.h"
#include "NetHelper.h"

NetHelper* GNetHelper = NULL ;

NetHelper::NetHelper(bool serverMode, char* serverAddr) : m_PeerAddrLen(0), m_Socket(NULL), m_IsServerMode(serverMode), m_IsPeerLinked(false)
{
	strcpy_s(m_TargetAddr, serverAddr) ;
}

NetHelper::~NetHelper()
{
	closesocket(m_Socket) ;
	WSACleanup() ;
}


bool NetHelper::Initialize()
{
	/// Socket √ ±‚»≠ 
	WSADATA wsa ;
	if ( WSAStartup(MAKEWORD(2, 2), &wsa) != 0 )
		return false ;

	m_Socket = socket(AF_INET, SOCK_DGRAM, 0) ;
	if (m_Socket == INVALID_SOCKET)
		return false ;

	return true ;
}

 
bool NetHelper::DoHandShake()
{
	char ioBuf[BUF_SIZE] = {0, } ;

	SOCKADDR_IN serveraddr ;
	ZeroMemory(&serveraddr, sizeof(serveraddr)) ;
	serveraddr.sin_family = AF_INET ;
	serveraddr.sin_port = htons(SERVER_PORT_NUM) ;

	if (m_IsServerMode)
	{

		BOOL on = TRUE ;
		int retval = setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) ;
		if (retval == SOCKET_ERROR)
			return false ;
	
		serveraddr.sin_addr.s_addr = htonl(INADDR_ANY) ;
		retval = bind(m_Socket, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) ;
		if (retval == SOCKET_ERROR)
			return false ;


		m_PeerAddrLen = sizeof(m_PeerAddrIn) ;
		retval = recvfrom(m_Socket, ioBuf, BUF_SIZE, 0, (SOCKADDR*)&m_PeerAddrIn, &m_PeerAddrLen) ;
		if (retval == SOCKET_ERROR)
		{
			MessageBox(NULL, L"ERROR: first recvfrom()", L"ERROR", MB_OK) ;
			return false ;
		}

		if (!strncmp(ioBuf, "CONNECT", 7))
		{
			sprintf_s(ioBuf, "SUCCESS") ;
			retval = sendto(m_Socket, ioBuf, strlen(ioBuf), 0, (SOCKADDR*)&m_PeerAddrIn, sizeof(m_PeerAddrIn)) ;
			if (retval == SOCKET_ERROR)
			{
				MessageBox(NULL, L"ERROR: sendto(SUCCESS)", L"ERROR", MB_OK) ;
				return false ;
			}

			m_IsPeerLinked = true ;
		}
		else
		{
			MessageBox(NULL, L"ERROR: INVALID CONNECT!!", L"ERROR", MB_OK) ;
			return false ;
		}

	}
	else
	{
		serveraddr.sin_addr.s_addr = inet_addr(m_TargetAddr) ;

		sprintf_s(ioBuf, "CONNECT");
		int retval = sendto(m_Socket, ioBuf, strlen(ioBuf), 0, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) ;
		if (retval == SOCKET_ERROR)
		{
			MessageBox(NULL, L"ERROR: first sendto(CONNECT)", L"ERROR", MB_OK) ;
			return false ;
		}

		m_PeerAddrLen = sizeof(m_PeerAddrIn) ;
		retval = recvfrom(m_Socket, ioBuf, BUF_SIZE, 0, (SOCKADDR*)&m_PeerAddrIn, &m_PeerAddrLen) ;
		if (retval == SOCKET_ERROR)
		{
			MessageBox(NULL, L"ERROR: recvfrom(SUCCESS)", L"ERROR", MB_OK) ;
			return false ;
		}

		if (!strncmp(ioBuf, "SUCCESS", 7))
		{
			m_IsPeerLinked = true ;
		}
		else
		{
			MessageBox(NULL, L"ERROR: INVALID SERVER!!", L"ERROR", MB_OK) ;
			return false ;
		}
	}

	return true ;
}


bool NetHelper::SendKeyStatus(const PacketKeyStatus& sendKeys)
{

	/// SEND
	int retval = sendto(m_Socket, (char*)&sendKeys, sizeof(PacketKeyStatus), 0, (SOCKADDR*)&m_PeerAddrIn, sizeof(m_PeerAddrIn)) ;
	if (retval == SOCKET_ERROR)
	{
		MessageBox(NULL, L"ERROR: sendto() in LOOP!", L"ERROR", MB_OK) ;
		return false ;
	}
	
	return true ;
}

bool NetHelper::RecvKeyStatus(OUT PacketKeyStatus& recvKeys)
{
	/// RECEIVE
	int retval = recvfrom(m_Socket, (char*)&recvKeys, sizeof(PacketKeyStatus), 0, (SOCKADDR*)&m_PeerAddrIn, &m_PeerAddrLen) ;
	if (retval == SOCKET_ERROR)
	{
		MessageBox(NULL, L"ERROR: recvfrom() in LOOP!", L"ERROR", MB_OK) ;
		return false ;
	}

	return true ;
}
