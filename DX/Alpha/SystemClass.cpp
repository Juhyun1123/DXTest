#include "stdafx.h"
#include "SystemClass.h"

SystemClass::SystemClass():
	input(nullptr), graphics(nullptr)
{ }

SystemClass::SystemClass(const SystemClass&) { }

SystemClass::~SystemClass() { }

bool SystemClass::Init()
{
	int screenWidth = 0, screenHeight = 0;
	bool result;

	// win api를 이용해 초기화
	InitWindows(screenWidth, screenHeight);

	// input 객체 생성 (유저의 키보드 처리)
	input = new InputClass();
	if (!input) { return false; }

	input->Init();

	// graphics 객체 생성 (그래픽 처리)
	graphics = new GraphicsClass();

	result = graphics->Init(screenWidth, screenHeight, hwnd);
	if (!result) { return false; }
	
	return true;
}

void SystemClass::Shutdown()
{
	// graphics 객체 해제
	if (graphics)
	{
		graphics->Shutdown();
		delete graphics;
		graphics = nullptr;
	}

	// input 객체 해제
	if (input)
	{
		delete input;
		input = nullptr;
	}

	// 창 종료
	ShutdownWindows();
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// 메세지 구조체 초기화
	ZeroMemory(&msg, sizeof(MSG));
	
	// 종료 메세지를 받을 때까지 루프 반복
	done = false;

	while (!done)
	{
		// 윈도우 메세지 처리
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// 기본 메세지 처리
		default:
			return DefWindowProc(hwnd, umsg, wparam, lparam);
	}
}

bool SystemClass::Frame()
{
	bool result;

	// 그래픽 작업 실행
	result = graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::InitWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc{ };
	DEVMODE dmScreenSettings;
	DWORD style = WS_OVERLAPPEDWINDOW;
	int posX, posY;

	// 외부 포인터를 이 객체로 설정 (윈도우 프로그램 핸들러를 현재 객체로 설정하는건가?)
	ApplicationHandle = this;

	// 이 어플리케이션의 인스턴스 가져오기.
	hinstance = GetModuleHandle(nullptr);

	// 어플리케이션 이름 설정
	applicationName = L"Alpha Engine";

	// 윈도우 클래스를 기본으로 설정
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszClassName = applicationName;
	wc.lpszMenuName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);


	// 윈도우 클래스 등록
	RegisterClassEx(&wc);

	// 모니터 화면의 해상도 가져오기
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		// 풀 스크린 세팅
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long>(screenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long>(screenHeight);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// 풀스크린에 맞는 디스플레이 설정
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// 윈도우의 위치를 화면의 왼쪽위로 지정
		posX = posY = 0;

		// 스타일 재지정 (전체 화면, 종료 화면 비활성)
		style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP;
	}
	else
	{
		// 위도우 모드 세팅

		// 화면 사이즈 조정 (800 * 600)
		screenWidth = 800;
		screenHeight = 600;

		// 창을 모니터 중앙에 오도록 설정
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// 설정한 내용으로 창을 생성하고 해당 핸들 획득
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW, applicationName, applicationName, style,
		posX, posY, screenWidth, screenHeight, nullptr, nullptr, hinstance, nullptr
	);

	// 윈도우를 화면에 표시하고 포커스 부여
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	//ShowCursor(false);
	ShowCursor(true);
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	// 풀 슼린 모드를 빠져나올때 디스플레이 설정 변경
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// 창 제거 
	DestroyWindow(hwnd);
	hwnd = nullptr;

	// 어플리케이션 인스턴스 제거 
	UnregisterClass(applicationName, hinstance);
	hinstance = nullptr;

	// 이 클래스에 대한 외부 포인터 참조 제거
	ApplicationHandle = nullptr;
}

LRESULT WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// 윈도우 제거 확인
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// 윈도우 닫힘 확인
	case WM_CLOSE:
		PostQuitMessage(0);

	default:
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}