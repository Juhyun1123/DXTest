#include "stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"
#include "CameraClass.h"
#include "ModelClass.h"
#include "ColorshaderClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Init(int screenWidth, int screenHeight, HWND hwnd)
{
    // D3D 객체 생성
    D3D = new D3DClass;

    if (!D3D)
    {
        return false;
    }

    // D3D 초기화
    if (!D3D->Init(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR))
    {
        return false;
    }

    Camera = new CameraClass;
    if (!Camera)
    {
        return false;
    }

    Camera->SetPosition(0.0f, 0.0f, -10.0f);

    Model = new ModelClass;
    if (!Model)
    {
        return false;
    }

    if (!Model->Init(D3D->GetDevice()))
    {
        MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
        return false;
    }

    ColorShader = new ColorShaderClass;
    if (!ColorShader)
    {
        return false;
    }

    if (!ColorShader->Init(D3D->GetDevice(), hwnd))
    {
        MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
        return false;
    }


    return true;
}

void GraphicsClass::Shutdown()
{
    // ColorShader 객체 반환
    if (ColorShader)
    {
        ColorShader->Shutdown();
        delete ColorShader;
        ColorShader = nullptr;
    }

    // Model 객체 반환
    if (Model)
    {
        Model->Shutdown();
        delete Model;
        Model = nullptr;
    }

    // Camera 객체 반환
    if (Camera)
    {
        delete Camera;
        Camera = nullptr;
    }

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

    // 카메라 위치에 따라 뷰 행렬 생성
    Camera->Render();

    // 카메라 및 D3D 객체에서 월드, 뷰, 투영 행렬 가져오기
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
    D3D->GetWorldMatrix(worldMatrix);
    Camera->GetViewMatrix(viewMatrix);
    D3D->GetProjectionMatrix(projectionMatrix);

    // 모델 정점과 인덱스 버퍼를 그래픽 파이프 라인에 배치해 드로잉 준비
    Model->Render(D3D->GetDeviceContext());

    // 색상 쉐이더를 사용해 모델링 렌더링
    if (!ColorShader->Render(D3D->GetDeviceContext(),
        Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix))
    {
        return false;
    }

    // 버퍼 내용 화면 표시
    D3D->EndScene();

    return true;
}

