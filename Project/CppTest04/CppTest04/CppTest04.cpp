// CppTest04.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <map>
#include <string>
using namespace std;

class CTest
{
public :
	static int m_a;
	int m_b;
	CTest() : m_b(0)
	{
		m_a++;
		m_b = m_a;
	}
};

int CTest::m_a = 0;

int main()
{
	map<wstring, int> myMap;

	//아이템 추가하기
	myMap.insert(pair<wstring, int>(L"Test", 100));
	myMap.insert(pair<wstring, int>(L"Abc", 200));

	myMap[L"def"] = 300;

	//맵 순화하기
	for (auto iter = myMap.begin(); iter != myMap.end(); ++iter)
		wcout << (*iter).first.c_str() << " = "
		<< (*iter).second << endl;
	wcout << endl;

	//키 값으로 아이템 찾기
	auto iter = myMap.find(L"Test");
	if (iter != myMap.end())
		wcout << "Find : " << (*iter).first.c_str() << " " << iter->second << endl;
	wcout << endl;

	//아이템 삭제하기
	myMap.erase(L"Abc");
	
	for (auto iter = myMap.begin(); iter != myMap.end(); ++iter)
		wcout << (*iter).first.c_str() << " = " << (*iter).first.c_str() << " = " << (*iter).second << endl;
	return 0;
}

// 프로그램 실행: <Ctrl+F5> 또는 [디버그] > [디버깅하지 않고 시작] 메뉴
// 프로그램 디버그: <F5> 키 또는 [디버그] > [디버깅 시작] 메뉴

// 시작을 위한 팁: 
//   1. [솔루션 탐색기] 창을 사용하여 파일을 추가/관리합니다.
//   2. [팀 탐색기] 창을 사용하여 소스 제어에 연결합니다.
//   3. [출력] 창을 사용하여 빌드 출력 및 기타 메시지를 확인합니다.
//   4. [오류 목록] 창을 사용하여 오류를 봅니다.
//   5. [프로젝트] > [새 항목 추가]로 이동하여 새 코드 파일을 만들거나, [프로젝트] > [기존 항목 추가]로 이동하여 기존 코드 파일을 프로젝트에 추가합니다.
//   6. 나중에 이 프로젝트를 다시 열려면 [파일] > [열기] > [프로젝트]로 이동하고 .sln 파일을 선택합니다.
