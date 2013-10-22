#pragma once

#include <winsock2.h>
#include "Config.h"
#include "PacketType.h"

class NetHelper
{
public:
	NetHelper(bool serverMode, char* serverAddr) ;
	~NetHelper() ;

	bool Initialize() ;

	/// 2 컴퓨터간 통신 확인
	bool DoHandShake() ;

	bool IsPeerLinked() const { return m_IsPeerLinked ; }
	bool IsServerMode() const { return m_IsServerMode ; }

	bool SendKeyStatus(const PacketKeyStatus& sendKeys) ;
	bool RecvKeyStatus(OUT PacketKeyStatus& recvKeys) ;

private:

	bool			m_IsServerMode ;
	bool			m_IsPeerLinked ;

	SOCKET			m_Socket ;
	SOCKADDR_IN		m_PeerAddrIn ;
	int				m_PeerAddrLen ;

	char			m_TargetAddr[MAX_STRING] ;
} ;


extern NetHelper* GNetHelper ;

