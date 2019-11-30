#include <d3d9.h>
#include <d3dx9.h>
#include <dinput.h>

#include "CInput.h"


CInput::CInput(HWND hwnd, HINSTANCE hInstance)
	:m_lpDInput(NULL),m_lpKeyBoard(NULL)

{
	InitInput(hwnd, hInstance);
}

CInput::~CInput()
{
	ShutdownInput();
}

int CInput::InitInput(HWND hwnd, HINSTANCE hInstance)
{
	auto result = DirectInput8Create(hInstance,
		DIRECTINPUT_VERSION
		, IID_IDirectInput8
		, (LPVOID*)& m_lpDInput
		, NULL);

	if (FAILED(result))
		return -1;

	//키보드 디바이스 성정하기
	result = m_lpDInput->CreateDevice(GUID_SysKeyboard
		, &m_lpKeyBoard, NULL);
	if (FAILED(result))
		return -2;

	//키보드 데이터 포맷 설정
	result = m_lpKeyBoard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result))
		return -3;

	//키보드 독점 사용에 대해서 알리기
	result = m_lpKeyBoard->SetCooperativeLevel(hwnd
		, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result))
		return -4;

	//키보드 사용
	result = m_lpKeyBoard->Acquire();
	if (FAILED(result))
		return -5;

	return 1;
}

void CInput::ShutdownInput()
{
	if (m_lpKeyBoard != NULL) {
		m_lpKeyBoard->Unacquire();
		m_lpKeyBoard->Release();
		m_lpKeyBoard = NULL;
	}
	
	if(m_lpDInput!= NULL){
		m_lpDInput->Release();
		m_lpDInput = NULL;
	}
}

bool CInput::ReadKeyBoard()
{
	auto result = m_lpKeyBoard->GetDeviceState(sizeof(m_KeyboardState)
		, (LPVOID)& m_KeyboardState);

	if (FAILED(result)) {
		if (result == DIERR_INPUTLOST ||
			result == DIERR_NOTACQUIRED)
		m_lpKeyBoard->Acquire();
		else
			return false;
	}
	return true;
}

void CInput::GetInputDir(D3DXVECTOR3& vDir)
{
	vDir = { 0.f , 0.f, 0.f };

	vDir.x += IsKeyPressed(DIK_LEFTARROW) ? -1.f : 0.f;
	vDir.x += IsKeyPressed(DIK_RIGHTARROW) ? 1.f : 0.f;

}

bool CInput::IsSpaceKeyPressed()
{
	static DWORD dwPrevTime = 0;
	unsigned long dwCurTime = GetTickCount();
	int dt = dwCurTime - dwPrevTime;

	if ((dt > 200) && (m_KeyboardState[DIK_SPACE] & 0x80)) {
		m_KeyboardState[DIK_SPACE] = 0;
		dwPrevTime = dwCurTime;
		return true;
	}
	return false;
}
