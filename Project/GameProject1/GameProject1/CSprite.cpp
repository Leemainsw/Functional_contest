#include <d3d9.h>
#include <d3dx9.h>
#include "CSprite.h"


bool CSprite::Creat(LPDIRECT3DDEVICE9 pD3DDevice)
{
	HRESULT hr = D3DXCreateSprite(pD3DDevice, &m_Sprite);
	if (FAILED(hr))
		return true;
	
	return false;
}

void CSprite::Release()
{
	if (m_Sprite != NULL) {
		m_Sprite->Release();
		m_Sprite = NULL;
	}
}

void CSprite::Draw(LPDIRECT3DTEXTURE9 pTex
					, const D3DXVECTOR3 cen
					, const D3DXVECTOR3 pos)
{
	m_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	m_Sprite->Draw(pTex, NULL, &cen, &pos,
		D3DCOLOR_XRGB(255, 255, 255));
	m_Sprite->End();
}

CSprite::CSprite(LPDIRECT3DDEVICE9 pD3DDevice) {
	Creat(pD3DDevice);
}

CSprite::CSprite()
	:m_Sprite(NULL)
{
	m_Sprite = NULL;
}


CSprite::~CSprite()
{
	Release(); //소멸자에 넣으면 내가 release 깜빡해도 
}
