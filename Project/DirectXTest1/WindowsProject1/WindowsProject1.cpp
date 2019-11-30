// WindowsProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "WindowsProject1.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <stdlib.h>
#include <time.h>
#include "C:/Program%20Files%20(x86)/Microsoft%20DirectX%20SDK%20(June%202010)/Include/d3dx9math.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND&);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LPDIRECT3D9 g_pD3D;
LPDIRECT3DDEVICE9 g_pD3DDevice;
D3DCOLOR g_ClearColor = D3DCOLOR_XRGB(0, 0, 255);

LPD3DXSPRITE g_Sprite;
LPDIRECT3DTEXTURE9 g_Texture[2];
D3DXVECTOR3 g_Snake[21];// = D3DXVECTOR3(100, 100, 0);
int g_BodyCount = 1;
bool DirTF = false;
D3DXVECTOR3 g_HeadPos = D3DXVECTOR3(100, 100, 0);
D3DXVECTOR3 g_Dir = D3DXVECTOR3(1, 0, 0);

D3DXVECTOR3 g_BlockPos[20];

bool InitDirect3D(HWND hwnd);
void ReleaseDirect3D();
void DrawSprite(D3DXVECTOR3& center, D3DXVECTOR3& position, int index);
void Render();

void CheckWorldBound();
void CheckBlocks();
void CheckSnakeBodys();

LPD3DXSPRITE CreateSprite(LPDIRECT3DDEVICE9 pD3DDevice);
void ReleaseSprite(LPD3DXSPRITE Sprite);

LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 pD3DDevice, int width, int height, D3DCOLOR color);
void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 여기에 코드를 입력합니다.

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	HWND hWnd = NULL;

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow, hWnd))
	{
		return FALSE;
	}

	InitDirect3D(hWnd);

	g_Sprite = CreateSprite(g_pD3DDevice);
	g_Texture[0] = CreateTexture(g_pD3DDevice, 20, 20,D3DCOLOR_ARGB(100,255,0,0));
	g_Texture[1] = CreateTexture(g_pD3DDevice, 20, 20,D3DCOLOR_ARGB(255,255,174,200));


	srand((unsigned int)time(NULL));
	
	for(D3DXVECTOR3& pos : g_BlockPos)
		 pos = D3DXVECTOR3(-999, -999, 0);
		//pos = D3DXVECTOR3(rand() % 32 * 20, rand() % 24 * 20, 0);

	g_BlockPos[0] = D3DXVECTOR3(rand() % 32 * 20, rand() % 24 * 20, 0);
	g_Snake[0] = D3DXVECTOR3(100, 100, 0);

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


				CheckBlocks();
				CheckWorldBound();
				CheckSnakeBodys();

				for (int i = g_BodyCount - 1; i >= 0; --i) {
					g_Snake[i] = g_Snake[i - 1];
				}
				g_Snake[0] = g_HeadPos;

				//for (int i = 0; i < g_BodyCount; ++i) {
				//	g_Snake[i] = g_HeadPos - g_Dir * (float)i * 20.f;
				//}


			}
			dropFrame++;
			Render();
		}
	}

	ReleaseTexture(g_Texture[1]);
	ReleaseTexture(g_Texture[0]);
	ReleaseSprite(g_Sprite);
	ReleaseDirect3D();

	return (int)msg.wParam;
}

void CheckSnakeBodys() {
	for (int i = 1; i < g_BodyCount; ++i) {
		auto body = g_Snake[i];
		if (g_HeadPos.x > body.x - 20.f && g_HeadPos.x < body.x + 20.f
			&& g_HeadPos.y > body.y - 20.f && g_HeadPos.y < body.y + 20.f) {
			g_ClearColor = D3DCOLOR_XRGB(0, 0, 0);
			g_BodyCount = 1;
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

void CheckBlocks() {
	g_ClearColor = D3DCOLOR_XRGB(0, 0, 255);

	for (D3DXVECTOR3& blockPos : g_BlockPos) {
		if (g_HeadPos.x > blockPos.x - 20.0f && g_HeadPos.x < blockPos.x + 20.0f
			&& g_HeadPos.y > blockPos.y - 20.0f && g_HeadPos.y < blockPos.y + 20.0f) {

			g_BodyCount++;

			g_BlockPos[g_BodyCount] = D3DXVECTOR3(rand() % 32 * 20, rand() % 24 * 20, 0);


			//g_Pos[g_BodyCount - 1]=
			g_ClearColor = D3DCOLOR_XRGB(255, 0, 0);
			blockPos = D3DXVECTOR3(-999, -999, 0);
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;//MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& rhWnd)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      0, 0, 640,480, nullptr, nullptr, hInstance, nullptr);

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
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {

	case WM_KEYDOWN: {
		
		if (g_Dir != D3DXVECTOR3(1, 0, 0)) {
			if (wParam == VK_LEFT) {
				g_Dir = D3DXVECTOR3(-1, 0, 0);
			}
		}

		if (g_Dir != D3DXVECTOR3(-1, 0, 0)) {
			if (wParam == VK_RIGHT) {
				g_Dir = D3DXVECTOR3(1, 0, 0);
			}
		}
		if (g_Dir != D3DXVECTOR3(0, 1, 0)) {
			if (wParam == VK_UP) {
				g_Dir = D3DXVECTOR3(0, -1, 0);
			}
		}
		if (g_Dir != D3DXVECTOR3(0, -1, 0)) {
			if (wParam == VK_DOWN) {
				g_Dir = D3DXVECTOR3(0, 1, 0);
			}
		}
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
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (g_pD3D == NULL)
		return false;

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));

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

	return true;
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

void DrawSprite(D3DXVECTOR3 &center, D3DXVECTOR3 &position, int index)
{
	/*D3DXMATRIX mat;
	D3DXVECTOR2 pos(position.x, position.y);
	D3DXVECTOR2 scale(1.f, 1.f);
	D3DXMatrixIdentity(&mat);
	D3DXMatrixAffineTransformation(&mat, NULL, 0.f, &scale, NULL, 0.7f, &pos);*/

	g_Sprite->Begin(D3DXSPRITE_ALPHABLEND);
	//g_Sprite->SetTransform(&mat);
	g_Sprite->Draw(g_Texture[index], NULL, &center, &position, D3DCOLOR_XRGB(255, 255, 255));
	g_Sprite->End();
}

void Render()
{
	if (g_pD3DDevice == NULL)
		return;

	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_ClearColor, 1.0f, 0);

	if (SUCCEEDED(g_pD3DDevice->BeginScene()))
	{
		D3DXVECTOR3 center(10, 10, 0);
		D3DXVECTOR3 position(100, 100, 0);

		//첫 번째 박스
			for(auto pos : g_BlockPos)
				DrawSprite(center, pos,1);

		//position += D3DXVECTOR3(50, 50, 0);
		
		//두 번째 박스
		for(int i=g_BodyCount-1;  i>=0; --i)
		DrawSprite(center, g_Snake[i],0);

		g_pD3DDevice->EndScene();
	}
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

LPD3DXSPRITE CreateSprite(LPDIRECT3DDEVICE9 pD3DDevice)
{
	LPD3DXSPRITE pSpr;
	auto hr = D3DXCreateSprite(pD3DDevice, &pSpr);
	if (FAILED(hr))
		return NULL;

	return pSpr;
}

LPDIRECT3DTEXTURE9 CreateTexture(LPDIRECT3DDEVICE9 pD3DDevice, int width, int height, D3DCOLOR color)
{
	LPDIRECT3DTEXTURE9 tex;
	pD3DDevice->CreateTexture(width, height, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &tex, 0);

	D3DLOCKED_RECT rect;
	tex->LockRect(0, &rect, 0, D3DLOCK_DISCARD);
	DWORD* pColor = (DWORD*)rect.pBits;

	D3DCOLOR lineColor = D3DCOLOR_ARGB(255,255,255,0);

	for (int i = 0; i < width * height; ++i)
		pColor[i] = color; //D3DCOLOR_ARGB(100, 255, 0, 0);

	

	for (int x = 0; x < width; ++x) {
		pColor[x] = lineColor;
		pColor[x+1] = lineColor;
		pColor[x+2] = lineColor;

		pColor[x + (height - 3) * width] = lineColor;
		pColor[x + (height - 2) * width] = lineColor;
		pColor[x + (height - 1) * width] = lineColor;

	}

	for (int y = 0; y < height; ++y) {
		pColor[y*width] = lineColor;
		pColor[1 + y * width] = lineColor;
		pColor[2 + y * width] = lineColor;

		pColor[ width - 1 + y  * width] = lineColor;
		pColor[ width - 2 + y * width] = lineColor;
		pColor[ width - 3 + y * width] = lineColor;

	}

	tex->UnlockRect(0);
	
	return tex;
}

void ReleaseTexture(LPDIRECT3DTEXTURE9 Texture)
{
	if (Texture != NULL)
		Texture->Release();
}

void ReleaseSprite(LPD3DXSPRITE Sprite)
{
	if (Sprite != NULL)
		Sprite->Release();
}
