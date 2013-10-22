#include "stdafx.h"
#include "Player.h"
#include "Config.h"
#include "NetHelper.h"

Player::Player()
{
	memset(m_PrevKeyState, false, MAX_KEY_TYPE) ;
	memset(m_NowKeyState, false, MAX_KEY_TYPE) ;

	m_Pos.x = 0.f ;
	m_Pos.y = 0.f ;

	m_Vel = 1.f ;
}

void Player::UpdatePosition(float diffX, float diffY)
{
	m_Pos.x += diffX * m_Vel ;
	m_Pos.y += diffY * m_Vel ;

	if ( m_Pos.x < -1 * (WINDOW_WIDTH / 2.f) )
	{
		m_Pos.x += WINDOW_WIDTH ;
	}
	else if ( m_Pos.x > (WINDOW_WIDTH / 2.f) )
	{
		m_Pos.x -= WINDOW_WIDTH ;
	}

	if ( m_Pos.y < -1 * (WINDOW_HEIGHT / 2.f) )
	{
		m_Pos.y += WINDOW_HEIGHT ;
	}
	else if ( m_Pos.y > (WINDOW_HEIGHT / 2.f) )
	{
		m_Pos.y -= WINDOW_HEIGHT ;
	}
}



KeyState Player::GetKeyState(int key) const
{
	if ( m_PrevKeyState[key] == false && m_NowKeyState[key] == true )
	{
		return KEY_DOWN;
	}
	else if ( m_PrevKeyState[key] == true && m_NowKeyState[key] == true )
	{
		return KEY_PRESSED;
	}
	else if ( m_PrevKeyState[key] == true && m_NowKeyState[key] == false )
	{
		return KEY_UP;
	}

	return KEY_NOTPRESSED;
}

void Player::UpdateStateByPeerInput(int frameNum)
{
	if ( !GNetHelper->IsPeerLinked() )
		return ;

	for (int i=0; i<MAX_KEY_TYPE; i++ )
	{
		m_PrevKeyState[i] = m_NowKeyState[i];
	}

	/// P2P 데이터 받아서 상태 업데이트
	PacketKeyStatus recvPkt ;
	GNetHelper->RecvKeyStatus(recvPkt) ;
	
	memcpy_s(m_NowKeyState, MAX_KEY_TYPE, recvPkt.mKeyStatus, MAX_KEY_TYPE) ;

	if ( recvPkt.mSequence != frameNum )
	{
		/// 여기 걸리면 프레임 빗나간 것이다..
		assert(false) ;
	}

	UpdateStatus() ;
}

void Player::UpdateStateByKeyInput(int frameNum)
{
	if ( !GNetHelper->IsPeerLinked() )
		return ;

	for (int i=0; i<MAX_KEY_TYPE; i++ )
	{
		m_PrevKeyState[i] = m_NowKeyState[i];

		if( ::GetKeyState(i) & 0x8000 )
		{
			m_NowKeyState[i] = true;
		}
		else
		{
			m_NowKeyState[i] = false;
		}
	}


	/// P2P 데이터 보낸 후에 업데이트
	PacketKeyStatus sendPkt ;
	sendPkt.mSequence = frameNum ;
	memcpy_s(sendPkt.mKeyStatus, MAX_KEY_TYPE, m_NowKeyState, MAX_KEY_TYPE) ;

	GNetHelper->SendKeyStatus(sendPkt) ;

	UpdateStatus() ;
}

void Player::UpdateStatus()
{
	if ( GetKeyState(VK_ADD) == KEY_DOWN || GetKeyState(VK_ADD) == KEY_PRESSED )
	{
		UpdateVelocity(1.05f) ;
	}
	if ( GetKeyState(VK_SUBTRACT) == KEY_DOWN || GetKeyState(VK_SUBTRACT) == KEY_PRESSED )
	{
		UpdateVelocity(0.95f) ;
	}


	if ( GetKeyState(VK_DOWN) == KEY_DOWN || GetKeyState(VK_DOWN) == KEY_PRESSED )
	{
		UpdatePosition(0, 10.f) ;
	}

	if ( GetKeyState(VK_UP) == KEY_DOWN || GetKeyState(VK_UP) == KEY_PRESSED )
	{
		UpdatePosition(0, -10.f) ;
	}

	if ( GetKeyState(VK_LEFT) == KEY_DOWN || GetKeyState(VK_LEFT) == KEY_PRESSED )
	{
		UpdatePosition(-10.f, 0) ;
	}

	if ( GetKeyState(VK_RIGHT) == KEY_DOWN || GetKeyState(VK_RIGHT) == KEY_PRESSED)
	{
		UpdatePosition(10.f, 0) ;
	}
}