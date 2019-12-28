// GameProject1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include <d3d9.h>
#include <d3dx9.h>
#include <stack>
#include <vector>
#include <algorithm>
#include <memory>

#include "framework.h"
#include "GameProject1.h"

using namespace std;

#include "CSprite.h"
#include "CTexture.h"
#include "CEntity.h"
#include "CGameObject.h"
#include "CInput.h"

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

#pragma comment(lib,"dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& hWnd);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


LPDIRECT3D9 g_pD3D;
LPDIRECT3DDEVICE9 g_pD3DDevice;
D3DCOLOR g_ClearColor = D3DCOLOR_XRGB(0, 0, 0);

//CGameObject *MonsterObject[20];
std::vector<CGameObject*> MonsterObject;

shared_ptr<CGameObject> PlayerObject;
//shared_ptr : <>안에 들어온 값을 sharedptr로 만들다.

//CGameObject* ShootObject[20];
std::vector<CGameObject*> ShootObject;
stack<CGameObject*> g_ShootPool; //총알 Stack
D3DXVECTOR3 MonsterDir = { 1.f, 0.f, 0.f };

CInput* g_Input;

//함수선언
void Render();
bool InitDirect3D(HWND hwnd);
void ReleaseDirect3D();
bool CheckWorldBound();
void SetMonsterDir(const D3DXVECTOR3& dir);

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
    LoadStringW(hInstance, IDC_GAMEPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

	HWND hWnd = NULL;

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow,hWnd))
    {
        return FALSE;
    }

	InitDirect3D(hWnd);

	//g_pTexture = new CTexture(g_pD3DDevice, _T("Airplane.bmp")); //create까지 실행
	//g_pSprite = new CSprite(g_pD3DDevice); //create까지 실행
	//g_pEntity = new CEntity(100, { 100,100,0 }, { 32,32,0 }, { 0,0,0 }); //speed,
	//g_GameObject[10] = new CGameObject();
	
	g_Input = new CInput(hWnd, hInstance);

	float y = 10.f;
	float x;
	for (int i = 0; i < 20; i++, x++) {
		if (i % 5 == 0) { y += 60.f; x = 0; }
		auto _MonsterObject = new CGameObject(g_pD3DDevice, _T("Monster.bmp"),{ 35.f + float(x * 64), y ,0.f });
		_MonsterObject->SetDir(MonsterDir);
		_MonsterObject->SetAlive(true);

		MonsterObject.push_back(_MonsterObject);
	}

	PlayerObject = make_shared<CGameObject>(
		new CGameObject(g_pD3DDevice, _T("Airplane.bmp")
			, { 320.f, 440.f ,0.f }));

	//PlayerObject = shared_ptr<CGameObject>(
	//	new CGameObject(g_pD3DDevice, _T("Airplane.bmp")
	//		, { 320.f, 440.f ,0.f })
	//	);

	PlayerObject->SetSpeed(200.f);
	PlayerObject->SetAlive(true);

	for (int i = 0; i < 50; i++) {
		auto _ShootObject = new CGameObject(g_pD3DDevice, _T("Bullet.bmp")
			, { 0,0,0 }, { 8.f, 16.f, 0.f }, { 0.f, -1.f, 0.f }, 200.f);
		//ShootObject[i]->SetAlive(false);
		ShootObject.push_back(_ShootObject);
		g_ShootPool.push(ShootObject[i]);
	}

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
		else {
			g_Input->ReadKeyBoard();
			if (g_Input->IsEscaperPressed())
				msg.message = WM_QUIT;
			
			D3DXVECTOR3 keyDir = { 0.f, 0.f, 0.f };

			//keyDir.x += g_Input->IsKeyPressed(DIK_LEFTARROW) ? -1.f : 0.f;
			//keyDir.x += g_Input->IsKeyPressed(DIK_RIGHTARROW) ? 1.f : 0.f;
			//keyDir.y += g_Input->IsKeyPressed(DIK_UPARROW) ? -1.f : 0.f;
			//keyDir.y += g_Input->IsKeyPressed(DIK_DOWNARROW) ? 1.f : 0.f;

			g_Input->GetInputDir(keyDir);


			static DWORD dwPrevTime = GetTickCount(); //항상 이전의 시간을 기록한다.
			DWORD dwCurTime = GetTickCount();
			DWORD dwDt = dwCurTime - dwPrevTime;
			float fDeltaTime = (float)(dwDt) * 0.001f;
			dwPrevTime = dwCurTime;

			PlayerObject->SetDir(keyDir);		
			PlayerObject->Update(fDeltaTime);

			for (size_t i = 0; i < MonsterObject.size(); i++) 
				MonsterObject[i]->Update(fDeltaTime);

			//for_each(MonsterObject.begin(), MonsterObject._Unchecked_begin(), [](auto go)) {
			//	go->Update(fDeltaTime);
			//};
			

			for(int i=0; i < ShootObject.size(); i++)
				if(ShootObject[i]->GetAlive())
					ShootObject[i]->Update(fDeltaTime);

			//화면 가장자리 도달했는지 체크
			for (int i = 0; i < MonsterObject.size(); i++)
			{
				if (MonsterObject[i]->GetAlive()) {
					auto pos = MonsterObject[i]->GetPos();
					if (pos.x > 640.f - 32.f)
					{
						MonsterDir = { -1.f,0.f,0.f };
						SetMonsterDir(MonsterDir);
						break;
					}
					else if (pos.x < 32.f)
					{
						MonsterDir = { 1.f, 0.f, 0.f };
						SetMonsterDir(MonsterDir);
						break;
					}
				}
			}

			//for (int i = 0; i < 20; i++) {
			//	//Shoot 발사체 처리
			//	if (ShootObject[i]->GetAlive() == false && g_Input->IsKeyPressed(DIK_SPACE))
			//	{
			//		auto pos = PlayerObject->GetPos();
			//		ShootObject[i]->SetPos(pos + D3DXVECTOR3(0, -32, 0));
			//		ShootObject[i]->SetAlive(true);
			//	}
			//}

			if (g_Input->IsSpaceKeyPressed())
			{
				if (!g_ShootPool.empty())
				{
					auto pShoot = g_ShootPool.top();
					auto pos = PlayerObject->GetPos();
					pShoot->SetPos(pos + D3DXVECTOR3(16, -32, 0));
					pShoot->SetAlive(true);

					g_ShootPool.pop();
				}
				if (!g_ShootPool.empty())
				{
					auto pShoot = g_ShootPool.top();
					auto pos = PlayerObject->GetPos();
					pShoot->SetPos(pos + D3DXVECTOR3(-16, -32, 0));
					pShoot->SetAlive(true);

					g_ShootPool.pop();
				}
			}

			for (int n = 0; n < ShootObject.size(); n++) {
				if (ShootObject[n]->GetAlive()){					
					auto pos = ShootObject[n]->GetPos();

					if (pos.y < 0.f)
						ShootObject[n]->SetAlive(false);

					for (int i = 0; i < MonsterObject.size(); i++) {
						if (MonsterObject[i]->GetAlive()) {

							auto MonPos = MonsterObject[i]->GetPos();

							if (pos.x > MonPos.x - (32.f + 8.f)
								&& pos.x < MonPos.x + (32 + 8.f)
								&& pos.y > MonPos.y - (32.f + 16.f)
								&& pos.y < MonPos.y + (32.f + 16.f))
							{
								MonsterObject[i]->SetAlive(false);
								ShootObject[n]->SetAlive(false);
								g_ShootPool.push(ShootObject[n]);
								break;
							}
						}
					}
				}
			}

			Render();
		}
	}

	//delete g_pEntity;
	//delete g_pTexture;
	//delete g_pSprite;
	for (int i = 0; i < ShootObject.size(); i++) 
		delete ShootObject[i];
	for (int i = 0; i < MonsterObject.size(); i++)
		delete MonsterObject[i];
	
	MonsterObject.clear();

	// delete PlayerObject; 
	delete g_Input;
	
	CTexture::ReleaseAll();

	ReleaseDirect3D();

    return (int) msg.wParam;
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_GAMEPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_GAMEPROJECT1);
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& hWnd)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

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
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//// 정보 대화 상자의 메시지 처리기입니다.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}
//


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

void Render() {
	if (g_pD3DDevice == NULL)
		return;

	g_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, g_ClearColor, 1.0f, 0);
	
	if (SUCCEEDED(g_pD3DDevice->BeginScene())) {
		for(int i=0; i< MonsterObject.size(); i++)
		MonsterObject[i]->Draw();
		PlayerObject->Draw();
		//g_pSprite->Draw(g_pTexture->GetTexture()
		//	, g_pEntity->GetCenter()
		//	, g_pEntity->GetPos());

		for(int i=0; i<ShootObject.size(); i++)
		ShootObject[i]->Draw();

		g_pD3DDevice->EndScene();
	}
	g_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

bool CheckWorldBound() {


	if (MonsterObject[4]->GetPos().x > 640.f - 40.f) {
		MonsterDir.x = -1.f;
		MonsterObject[0]->SetDir(MonsterDir);
		return true;
	}
		
	else if (MonsterObject[0]->GetPos().x < 40.f) {
		MonsterDir.x = 1.f;
		MonsterObject[0]->SetDir(MonsterDir);
		return true;
	}
	
	return false;
}

void SetMonsterDir(const D3DXVECTOR3& dir) 
{
	for (int n = 0; n < MonsterObject.size(); ++n)
	{
		MonsterObject[n]->SetPos(
			MonsterObject[n]->GetPos() + D3DXVECTOR3(0, 10, 0));
		MonsterObject[n]->SetDir(dir);
	}
}