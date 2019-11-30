// AppTest0713.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "AppTest0713.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	MessageBox(NULL, TEXT("Hello"), TEXT("APP"), MB_OK);
	return 0;
}
