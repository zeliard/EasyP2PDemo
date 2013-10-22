#include "stdafx.h"
#include "Player.h"
#include "P2PDemo.h"
#include "Scene.h"
#include "NetHelper.h"


HRESULT Scene::Initialize()
{
	return D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pFactory) ;
}

void Scene::CleanUp()
{
	DiscardDeviceDependentResources();

	// Discard device-independent resources.
	SafeRelease(&m_pFactory);
}

void Scene::Render(HWND hwnd)
{
	HRESULT hr = CreateGraphicsResources(hwnd);
	if (FAILED(hr))
		return;

	assert(m_pRenderTarget != NULL);

	/// 나는 키 입력을 기반으로 업데이트
	m_MyPlayer.UpdateStateByKeyInput(m_CurrentFrame) ;

	/// 상대는 네트웍 입력을 기반으로 업데이트
	m_PeerPlayer.UpdateStateByPeerInput(m_CurrentFrame) ;


	m_pRenderTarget->BeginDraw() ; 

	RenderScene() ;

	hr = m_pRenderTarget->EndDraw() ;
	if (hr == D2DERR_RECREATE_TARGET)
		DiscardDeviceDependentResources() ;
	
}

HRESULT Scene::Resize(int x, int y)
{
	HRESULT hr = S_OK;
	if (m_pRenderTarget)
	{
		hr = m_pRenderTarget->Resize( D2D1::SizeU(x, y) );
	}
	return hr;
}


HRESULT Scene::CreateGraphicsResources(HWND hwnd)
{
	HRESULT hr = S_OK;
	if (m_pRenderTarget == NULL)
	{
		RECT rc;
		GetClientRect(hwnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

		hr = m_pFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hwnd, size),
			&m_pRenderTarget
			);

		if (SUCCEEDED(hr))
		{
			hr = CreateDeviceDependentResources();
		}
	}
	return hr;
}


HRESULT Scene::CreateDeviceDependentResources()
{
	HRESULT hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0), &m_pStroke);
	if ( hr < 0 )
		return -1 ;

	hr = DWriteCreateFactory( DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_DWriteFactory), reinterpret_cast<IUnknown**>(&m_DWriteFactory) ) ;

	if ( hr < 0 )
		return -1 ;

	hr = m_DWriteFactory->CreateTextFormat( L"Arial", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
		30, L"en", &m_TextFormat ) ;

	return hr ;
}



void Scene::RenderScene()
{
	m_pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

	if ( !GNetHelper->IsPeerLinked() )
	{
		if ( GNetHelper->IsServerMode() )
			wcscpy_s(m_StringOut, L"SERVER MODE: PRESS ENTER TO HANDSHAKE WAIT") ;
		else
			wcscpy_s(m_StringOut, L"CLIENT MODE: PRESS ENTER TO HANDSHAKE START") ;

		m_pRenderTarget->DrawTextW(m_StringOut, wcslen(m_StringOut), m_TextFormat,
			D2D1::RectF(0,0,WINDOW_WIDTH,WINDOW_HEIGHT), m_pStroke ) ;

		return ;
	}

	/// 아래는 handshake (link) 되었을때부터 렌더링 할 것

	{
		D2D_POINT_2F pos ;
		pos.x = m_MyPlayer.GetPosition().x + (WINDOW_WIDTH / 2.f) ;
		pos.y = m_MyPlayer.GetPosition().y + (WINDOW_HEIGHT / 2.f) ;
		D2D1_ELLIPSE player = D2D1::Ellipse( pos, 30, 30) ;
		m_pRenderTarget->DrawEllipse(player, m_pStroke) ;
	}

	{
		D2D_POINT_2F pos ;
		pos.x = m_PeerPlayer.GetPosition().x + (WINDOW_WIDTH / 2.f) ;
		pos.y = m_PeerPlayer.GetPosition().y + (WINDOW_HEIGHT / 2.f) ;
		D2D1_RECT_F rect = D2D1::Rect(pos.x-20, pos.y-20, pos.x+20, pos.y+20) ;
		m_pRenderTarget->DrawRectangle(&rect, m_pStroke) ;
	}



	++m_CurrentFrame ;

	wsprintf(m_StringOut, L"[%d]", m_CurrentFrame) ;
	m_pRenderTarget->DrawTextW(m_StringOut, wcslen(m_StringOut), m_TextFormat,
		D2D1::RectF(0,0,WINDOW_WIDTH,WINDOW_HEIGHT), m_pStroke ) ;
}




void Scene::DiscardDeviceDependentResources()
{
	SafeRelease(&m_pStroke);
	SafeRelease(&m_pRenderTarget);
	SafeRelease(&m_DWriteFactory);
	SafeRelease(&m_TextFormat);
}
