#pragma once

#include <dinput.h>

class CInput
{
public:
	explicit CInput(HWND hwnd, HINSTANCE hInstance);
	virtual ~CInput();

	int InitInput(HWND hwnd, HINSTANCE hInstance);
	void ShutdownInput();

	bool ReadKeyBoard();

	void ClearKeyBudder()
	{
		ZeroMemory(m_KeyboardState, sizeof(m_KeyboardState));
	}


	bool IsEscaperPressed()
	{
		return (m_KeyboardState[DIK_ESCAPE] & 0x80) ? true : false;
		return false;
	}

	bool IsKeyPressed(int key)
	{
		return (m_KeyboardState[key] & 0x80) ? true : false;
	}

	void GetInputDir(D3DXVECTOR3& vDir);
	bool IsSpaceKeyPressed();

protected:
	LPDIRECTINPUT8 m_lpDInput;
	LPDIRECTINPUTDEVICE8 m_lpKeyBoard;
	unsigned char m_KeyboardState[256];
};

