#pragma once
#include <map>
#include <string>
using namespace std;

class CTexture
{
protected:
	static map<wstring, LPDIRECT3DTEXTURE9> s_TexturePool;
	LPDIRECT3DTEXTURE9 m_Texture;

public:
	//CreateTexture , LoadTexture, ReleaseTexture
	bool Create(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR* filename); //texture 객체 생성하는 
	void Release();

	LPDIRECT3DTEXTURE9 GetTexture() const{
		return m_Texture;
	}

	CTexture();
	CTexture(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR* filename);
	virtual ~CTexture();

	static void ReleaseAll();
};

