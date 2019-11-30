// DirectXTest1.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "DirectTest1.h"

#include <d3d9.h>
#include <d3dx9.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#define _CRT_SECURE_NO_WARNINGS
#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

LPDIRECT3D9 g_pD3D;
LPDIRECT3DDEVICE9 g_pD3DDevice;
D3DCOLOR g_ClearColor = D3DCOLOR_XRGB(0, 0, 255);

LPD3DXSPRITE g_Sprite;
LPDIRECT3DTEXTURE9 g_Texture[2];
LPD3DXFONT g_Font;

D3DXVECTOR3 g_Snake[769];
int g_BodyCount = 1;
int no = 4;
D3DXVECTOR3 g_HeadPos = D3DXVECTOR3(100, 100, 0);
D3DXVECTOR3 g_Dir = D3DXVECTOR3(1, 0, 0);

D3DXVECTOR3 g_BlockPos[1];

bool InitDirect3D(HWND hwnd);
void ReleaseDirect3D();
void DrawSprite(D3DXVECTOR3& center, D3DXVECTOR3& position, int index);
void Render();
void CheckWorldBound();
void ChekBlocks();
void CheckSnakeBodies();

LPD3DXSPRITE CreateSprite(LPDIRECT3DDEVICE9 pD3DDevice);
void ReleaseSprite(LPD3DXSPRITE Sprite);

LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 pD3DDevice, int width, int height, D3DCOLOR color);
void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture);
LPDIRECT3DTEXTURE9 LoadTexture(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR* filename);

LPD3DXFONT CreateFont(LPDIRECT3DDEVICE9 pD3DDevice, int height, int width);
void ReleaseFont(LPD3DXFONT font);

int score = 0;


int APIENTRY wWinMain(In HINSTANCE hInstance,
	In_opt HINSTANCE hPrevInstance,
	In LPWSTR    lpCmdLine,
	In int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DIRECTTEST1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	HWND hWnd = NULL;
	// 응용 프로그램 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	InitDirect3D(hWnd);

	g_Sprite = CreateSprite(g_pD3DDevice);
	g_Texture[0] = LoadTexture(g_pD3DDevice, TEXT("Body.bmp"));
	g_Texture[1] = CreateTexture(g_pD3DDevice, 20, 20, D3DCOLOR_ARGB(100, 0, 255, 0));

	g_Font = CreateFont(g_pD3DDevice, 24, 12);

	srand((unsigned int)time(NULL));

	for (D3DXVECTOR3& pos : g_BlockPos)
		pos = D3DXVECTOR3(-999, -999, 0);
	g_BlockPos[0] = D3DXVECTOR3(rand() % 32 * 20, rand() % 24 * 20, 0);
	g_HeadPos = D3DXVECTOR3(100, 100, 0);
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	// 기본 메시지 루프입니다:
	while (msg.message != WM_QUIT)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{

			static int dropFrame = 0;
			if (dropFrame >= 4) {
				dropFrame = 0;
				g_HeadPos = g_HeadPos + g_Dir * 20.f;

				CheckWorldBound();
				ChekBlocks();
				CheckSnakeBodies();

				for (int i = g_BodyCount - 1; i > 0; --i) {
					g_Snake[i] = g_Snake[i - 1];
				}
				g_Snake[0] = g_HeadPos;
			}
			dropFrame++;
			Render();
		}
	}

	ReleaseFont(g_Font);
	ReleaseTexture(g_Texture[0]);
	ReleaseTexture(g_Texture[1]);
	ReleaseSprite(g_Sprite);
	ReleaseDirect3D();

	return (int)msg.wParam;
}

void CheckSnakeBodies() {
	for (int i = 1; i < g_BodyCount; ++i) {
		auto body = g_Snake[i];
		if (g_HeadPos.x > body.x - 20.f && g_HeadPos.x<body.x + 20.f
			&& g_HeadPos.y>body.y - 20.f && g_HeadPos.y < body.y + 20.f)
		{
			g_BodyCount = 1;
			g_ClearColor = D3DCOLOR_XRGB(0, 0, 0);
		}
	}
}

void CheckWorldBound() {
	if (g_HeadPos.x > 640.f - 10.f)
		g_HeadPos.x = 640.f - 10.f;
	else if (g_HeadPos.x < 10.f)
		g_HeadPos.x = 10.f;

	if (g_HeadPos.y > 480.f - 10.f)
		g_HeadPos.y = 480.f - 10.f;
	else if (g_HeadPos.y < 10.f)
		g_HeadPos.y = 10.f;
}

void ChekBlocks() {
	g_ClearColor = D3DCOLOR_XRGB(0, 0, 255);
	for (D3DXVECTOR3& blockPos : g_BlockPos) {

		if (g_HeadPos.x > blockPos.x - 20.f && g_HeadPos.x<blockPos.x + 20.f
			&& g_HeadPos.y>blockPos.y - 20.f && g_HeadPos.y < blockPos.y + 20.f)
		{
			g_ClearColor = D3DCOLOR_XRGB(0, 255, 0);
			g_BodyCount++;
			blockPos = D3DXVECTOR3(rand() % 32 * 20, rand() % 24 * 20, 0);
			score += 100;

		}
	}
}

//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
// 
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DIRECTTEST1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;// MAKEINTRESOURCEW(IDC_DIRECTXTEST1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND & rhWnd)
{
	hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	rhWnd = hWnd;

	return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_LEFT && no != 1) { // 0
			g_Dir = D3DXVECTOR3(-1, 0, 0);
			no = 0;
		}
		else if (wParam == VK_RIGHT && no != 0) { // 1
			g_Dir = { 1, 0, 0 }; //morden c++
			no = 1;
		}
		if (wParam == VK_UP && no != 3) { // 2
			g_Dir = { 0, -1, 0 }; //morden c++
			no = 2;
		}
		else if (wParam == VK_DOWN && no != 2) { // 3
			g_Dir = D3DXVECTOR3(0, 1, 0);
			no = 3;
		}
	}
	break;
	case WM_KEYUP:
	{

	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

bool InitDirect3D(HWND hwnd)
{

	// DX 오브젝트 생성.
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL) {
		return false;
	}

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp)); // 구조체의 모든 변수를 0으로 초기화.

	d3dpp.Windowed = TRUE;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth = 640;
	d3dpp.BackBufferHeight = 480;

	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT
		, D3DDEVTYPE_HAL
		, hwnd
		, D3DCREATE_HARDWARE_VERTEXPROCESSING
		, &d3dpp
		, &g_pD3DDevice) == E_FAIL)
		return false;

	return false;
}

void ReleaseDirect3D()
{
	if (g_pD3DDevice != NULL)
		g_pD3DDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();

	g_pD3DDevice = NULL;
	g_pD3D = NULL;
}

void DrawSprite(D3DXVECTOR3 & center, D3DXVECTOR3 & position, int index)
{
	/*
	D3DXMATRIX mat;
	D3DXVECTOR2 pos(position.x, position.y);
	D3DXVECTOR2 scale(1.f, 1.f);
	D3DXMatrixIdentity(&mat);
	D3DXMatrixTransformation2D(&mat, NULL, 0.f, &scale, NULL, 0.7f, &pos);
	*/
	g_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	// g_Sprite->SetTransform(&mat);
	g_Sprite->Draw(g_Texture[index], NULL, &center, &position,
		D3DCOLOR_XRGB(255, 255, 255));
	g_Sprite->End();
}

void Render() {
	if (g_pD3DDevice == NULL)
		return;

	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_ClearColor, 1.0f, 0);

	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		D3DXVECTOR3 center(10, 10, 0);
		D3DXVECTOR3 position(100, 100, 0);

		// 1번째 박스
		for (auto pos : g_BlockPos) {
			DrawSprite(center, pos, 1);
		}

		position += D3DXVECTOR3(50, 50, 0);

		// 2번째 박스
		DrawSprite(center, g_Snake[0], 0);
		for (int i = g_BodyCount - 1; i >= 0; --i) {
			DrawSprite(center, g_Snake[i], 0);
		}

		RECT rc; //RECT는 Win32안에 있는애라서 따로 다른것들 안써줘도 된다.
		//SetRect(&rc, 0, 0, 640, 480);
		rc = { 0,0,640,480 };

		//g_Font->DrawTextW(NULL, TEXT("test : "),-1,&rc //-1은 글자전체를 다 출력해주는 것
		//	,DT_LEFT|DT_TOP,D3DCOLOR_XRGB(255,0,0));

		TCHAR score2[15];
		swprintf(score2, 15, TEXT("SCORE : %d"), score);
		g_Font->DrawTextW(NULL, score2, -1, &rc //-1은 글자전체를 다 출력해주는 것
			, DT_LEFT | DT_TOP, D3DCOLOR_XRGB(255, 0, 0));

		/*g_Font->DrawTextW(NULL, TEXT("test"), -1, &rc
			, DT_RIGHT | DT_TOP, D3DCOLOR_XRGB(255, 0, 0));
		g_Font->DrawTextW(NULL, TEXT("test"), -1, &rc
			, DT_CENTER | DT_VCENTER, D3DCOLOR_XRGB(255, 0, 0));*/

		g_pD3DDevice->EndScene();
	}

	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

LPD3DXSPRITE CreateSprite(LPDIRECT3DDEVICE9 pD3DDevice)
{
	LPD3DXSPRITE pSpr;
	HRESULT hr = D3DXCreateSprite(pD3DDevice, &pSpr);
	if (FAILED(hr))
		return NULL;

	return pSpr;
}

void ReleaseSprite(LPD3DXSPRITE Sprite)
{
	if (Sprite != NULL)
		Sprite->Release();
}

LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 pD3DDevice, int width, int height, D3DCOLOR color)
{
	LPDIRECT3DTEXTURE9 tex;
	pD3DDevice->CreateTexture(width, height, 1, 0,
		D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, 0);

	D3DLOCKED_RECT rect;
	tex->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
	DWORD* pColor = (DWORD*)rect.pBits;

	D3DCOLOR lineColor = D3DCOLOR_ARGB(255, 255, 255, 0);

	for (int i = 0; i < width * height; ++i)
		pColor[i] = color; // D3DCOLOR_ARGB(100, 255, 0, 0);

	// 가로선
	for (int x = 0; x < width; ++x) {
		pColor[x] = lineColor;
		pColor[x + 1 * width] = lineColor;
		pColor[x + 2 * width] = lineColor;
		pColor[x + (height - 3) * width] = lineColor;
		pColor[x + (height - 2) * width] = lineColor;
		pColor[x + (height - 1) * width] = lineColor;
	}

	// 세로선
	for (int y = 0; y < height; ++y) {
		pColor[y * width] = lineColor;
		pColor[1 + y * width] = lineColor;
		pColor[2 + y * width] = lineColor;
		pColor[width - 1 + y * width] = lineColor;
		pColor[width - 2 + y * width] = lineColor;
		pColor[width - 3 + y * width] = lineColor;
	}

	tex->UnlockRect(0);

	return tex;
}

LPDIRECT3DTEXTURE9 LoadTexture(LPDIRECT3DDEVICE9 pD3DDevice, const TCHAR * filename)
{
	LPDIRECT3DTEXTURE9 tex;
	auto hr = D3DXCreateTextureFromFileEx(pD3DDevice, filename
		, 0, 0, 0, 0
		, D3DFMT_A8R8G8B8
		, D3DPOOL_DEFAULT
		, D3DX_FILTER_NONE
		, D3DX_DEFAULT
		, D3DCOLOR_XRGB(0, 0, 0, 0)
		, NULL, NULL
		, &tex);

	if (FAILED(hr))
		return NULL;
	return tex;
}
void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture)
{
	if (Texture != NULL)
		Texture->Release();
}

LPD3DXFONT CreateFont(LPDIRECT3DDEVICE9 pD3DDevice, int height, int width)
{
	LPD3DXFONT font;
	D3DXFONT_DESC desc;
	ZeroMemory(&desc, sizeof(D3DXFONT_DESC));

	desc.CharSet = HANGUL_CHARSET;
	desc.Height = height;
	desc.Width = width;
	desc.Weight = FW_NORMAL;
	desc.Quality = DEFAULT_QUALITY;
	desc.MipLevels = 1;

	auto hr = D3DXCreateFontIndirect(pD3DDevice, &desc, &font);
	if (FAILED(hr))
		return NULL;
	return font;
}
void ReleaseFont(LPD3DXFONT font) {
	if (font != NULL)
		font->Release();
}