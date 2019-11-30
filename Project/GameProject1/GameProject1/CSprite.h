#pragma once
class CSprite
{

protected:
	//스프라이트
	LPD3DXSPRITE m_Sprite; //비행기

public:
	bool Creat(LPDIRECT3DDEVICE9 pD3DDevice);
	void Release();
	void Draw(LPDIRECT3DTEXTURE9 pTex, const D3DXVECTOR3 cen, const D3DXVECTOR3 pos);


	CSprite();
	CSprite(LPDIRECT3DDEVICE9 pD3DDevice);
	virtual ~CSprite();

};

