#pragma once
#include "PacketType.h"

enum KeyState
{
	KEY_DOWN, KEY_PRESSED, KEY_UP, KEY_NOTPRESSED
} ;


class Player
{
public:
	Player() ;

	const D2D_POINT_2F& GetPosition() const
	{
		return m_Pos ;
	}

	void	UpdateStateByKeyInput(int frameNum) ;
	void	UpdateStateByPeerInput(int frameNum) ;


private:
	KeyState GetKeyState(int key) const ;
	void UpdateStatus() ;
	void UpdatePosition(float diffX, float diffY) ;
	void UpdateVelocity(float multiply)  {	m_Vel *= multiply ; }

private:

	

	bool			m_PrevKeyState[MAX_KEY_TYPE] ;
	bool			m_NowKeyState[MAX_KEY_TYPE] ;

	D2D_POINT_2F	m_Pos ;
	float			m_Vel ;
} ;