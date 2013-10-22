#pragma once

#include "Config.h"
#include "Player.h"

class Scene
{
private:
	ID2D1Factory*			m_pFactory;
	ID2D1HwndRenderTarget*	m_pRenderTarget;

	ID2D1SolidColorBrush*	m_pStroke;
	IDWriteTextFormat*		m_TextFormat;
	IDWriteFactory*			m_DWriteFactory;

	HRESULT CreateGraphicsResources(HWND hwnd);
	HRESULT CreateDeviceDependentResources();
	void    DiscardDeviceDependentResources();
	void    CalculateLayout();
	void    RenderScene();



	wchar_t	m_StringOut[MAX_STRING] ;
	int		m_CurrentFrame ;

	//////////////////////////////////////////////////////////////////////////
	
	Player	m_MyPlayer ;
	Player	m_PeerPlayer ;


public:
	Scene() : m_pStroke(NULL),  m_pFactory(NULL),  m_pRenderTarget(NULL), m_CurrentFrame(0)
	{
		memset(m_StringOut, 0, sizeof(m_StringOut)) ;
	}

	~Scene()
	{
		CleanUp();
	}
	
	HRESULT Initialize();
	void Render(HWND hwnd);
	HRESULT Resize(int x, int y);
	void CleanUp() ;

} ;
