#include "stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"

GraphicsClass::GraphicsClass()
{
    D3D = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Init(int screenWidth, int screenHeight, HWND hwnd)
{
    D3D = new D3DClass();

    if (!D3D)
    {
        return false;
    }

    if (!D3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
    {
        return false;
    }

    return true;
}

void GraphicsClass::Shutdown()
{
    // D3D 객체 반환
    if (D3D)
    {
        D3D->Shutdown();
        delete D3D;
        D3D = nullptr;
    }
}

bool GraphicsClass::Frame()
{
    if (!Render())
    {
        return false;
    }

    return true;
}

bool GraphicsClass::Render()
{
    // 씬 그리기 시작전 버퍼 내용 지우기
    D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

    // 버퍼 내용 화면 표시
    D3D->EndScene();

    return true;
}
