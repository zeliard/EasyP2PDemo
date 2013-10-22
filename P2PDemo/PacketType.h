#pragma once

enum PacketTypes
{
	PKT_NONE	= 0,
	PKT_TEST	= 1,
	PKT_STATUS	= 2,
} ;

#define MAX_PAYLOAD		512
#define MAX_KEY_TYPE	256

#pragma pack(push, 1)

struct PacketTest
{
	PacketTest() : mType(PKT_TEST), mSequence(-1)
	{
		memset(mPayLoad, 0, MAX_PAYLOAD) ;
	}

	short	mType ;
	int		mSequence ;
	char	mPayLoad[MAX_PAYLOAD] ;
} ;

struct PacketKeyStatus
{
	PacketKeyStatus() : mType(PKT_STATUS), mSequence(-1)
	{
		memset(mKeyStatus, false, MAX_KEY_TYPE) ;
	}

	short	mType ;
	int		mSequence ;
	bool	mKeyStatus[MAX_KEY_TYPE] ;
	
} ;

#pragma pack(pop)