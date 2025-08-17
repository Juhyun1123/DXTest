#include "stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass() { }

D3DClass::D3DClass(const D3DClass&) { }

D3DClass::~D3DClass() { }

bool D3DClass::Init(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullScreen, float screenDepth, float screenNear)
{
    // 수직 동기화 상태 저장
    vsyncEnabled = vsync;

    // DX 그래픽 인터페이스 팩토리 생성
    IDXGIFactory* factory = nullptr;
    if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&factory))))
    {
        return false;
    }

    // 팩토리 객체를 사용해 첫번째 그래픽 카드 인터페이스 어뎁터 생성
    IDXGIAdapter* adapter = nullptr;
    if (FAILED(factory->EnumAdapters(0, &adapter))) // 0은 1번 그래픽 카드 (컴퓨터에 따라 내장 그래픽일 수 있음)
    {
        return false;
    }

    // 첫번째 출력 장치 어뎁터 생성
    IDXGIOutput* adapterOutput = nullptr;
    if (FAILED(adapter->EnumOutputs(0, &adapterOutput)))
    {
        return false;
    }

    // 출력에 대한 DXGI_FORMAT_R8G8B8A8_UNORM 표시 형식에 맞는 모드수 가져오기
    unsigned int numModes = 0;
    if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)))
    {
        return false;
    }

    // 가능한 모든 출력장치와 그래픽 카드 조합 (모드) 저장 리스트 생성
    DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
    if (!displayModeList)
    {
        return false;
    }

    // 디스플레이 모드 리스트 초기화
    if (FAILED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
    {
        return false;
    }

    // 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드 검색
    // 검색 성공시 모니터의 새로고침 비율을 분모와 분자값에 저장
    unsigned int numerator = 0; // 분자
    unsigned int denominator = 0; // 분모

    for (int i = 0; i < numModes; ++i)
    {
        if (displayModeList[i].Width == static_cast<unsigned>(screenWidth))
        {
            if (displayModeList[i].Height == static_cast<unsigned>(screenHeight))
            {
                numerator = displayModeList[i].RefreshRate.Numerator;
                denominator = displayModeList[i].RefreshRate.Denominator;
            }
        }
    }

    // 비디오카드의 구조체 획득
    DXGI_ADAPTER_DESC adapterDesc;
    if (FAILED(adapter->GetDesc(&adapterDesc)))
    {
        return false;
    }

    // 비디오카드 메모리 용량 단위를 MB 단위로 저장
    videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

    // 비디오 카드 이름 저장
    size_t stringLength = 0;
    if (wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128) != 0)
    {
        return false;
    }

    // 그래픽 카드 확인 용도
    const WCHAR* pwcsName; // LPCWSTR

    // required size
    int size = MultiByteToWideChar(CP_ACP, 0, videoCardDescription, -1, nullptr, 0);
    // allocate it
    pwcsName = new WCHAR[stringLength];
    MultiByteToWideChar(CP_ACP, 0, videoCardDescription, -1, (LPWSTR)pwcsName, size);
    // use it

    OutputDebugString(pwcsName);

    //delete it
    delete[] pwcsName;

    // 디스플레이 모드 리스트 해제
    delete[] displayModeList;
    displayModeList = nullptr;
    
    // 출력 어뎁터 해제
    adapterOutput->Release();
    adapterOutput = nullptr;

    // 어뎁터 해제
    adapter->Release();
    adapter = nullptr;



    // 스왑체인 구조체 초기화
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // 백버퍼 1개로 지정
    swapChainDesc.BufferCount = 1;

    // 백버퍼 넓이와 높이 설정
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // 33bit 서피스 설정
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

    // 백버퍼의 새로고침 비율 설정
    if (vsyncEnabled)
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // 백버퍼의 사용용도 지정
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // 렌더링에 사용될 윈도우 핸들 지정
    swapChainDesc.OutputWindow = hwnd;

    // 멀티샘플링 비활성
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // 창 모드 or 풀 스크린 모드 설정

    if (fullScreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    // 스캔 라인 순서 및 크기를 지정하지 않음으로 설정
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // 출력된 다음 백버퍼를 비우도록 지정
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    // 추가 옵션 플래그를 사용하지 않는다.
    swapChainDesc.Flags = 0;

    // 피처 레벨을 DX11로 설정
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

    // 스왑 체인, DX 장치 및 DX 장치 컨텍스트 생성
    if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, &featureLevel, 1,
        D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, nullptr, &deviceContext)))
    {
        return false;
    }

    // 백버퍼 포인터 획득
    ID3D11Texture2D* backBufferPtr = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&(backBufferPtr)))))
    {
        return false;
    }

    // 백버퍼 포인터로 렌더 타겟 뷰 생성
    if (FAILED(device->CreateRenderTargetView(backBufferPtr, nullptr, &renderTargetView)))
    {
        return false;
    }

    // 백버퍼 포인터 해제
    backBufferPtr->Release();
    backBufferPtr = nullptr;

    // 깊이 버퍼 구조체 초기화
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // 깊이 버퍼 구조체 작성
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // 설정된 깊이 버퍼 구조체를 사용해 깊이 버퍼 텍스처 생성
    if (FAILED(device->CreateTexture2D(&depthBufferDesc, nullptr, &depthStencilBuffer)))
    {
        return false;
    }

    // 스텐실 상태 구조체를 초기화
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // 스텐실 상태 구조체 작성
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // 픽셀 정면의 스텐실 설정
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // 픽셀 후면의 스텔실 설정
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // 깊이 스텔실 상태 설정
   /* if (FAILED(device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState))) {
        return false;
    }*/
    deviceContext->OMSetDepthStencilState(depthStencilState, 1);

    // 깊이 스텐실 뷰 구조체 초기화
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // 깊이 스텐실 뷰 구조체 설정
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // 깊이 스텐시 뷰 생성
    if (FAILED(device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView)))
    {
        return false;
    }

    // 렌더링 대상 뷰와 깊이 스텐실 버퍼를 출력 렌더 파이프라인에 바인딩
    deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    // 그려지는 폴리곤과 방법을 결정할 레스터 구조 설정
    D3D11_RASTERIZER_DESC rasterDesc;
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // 앞의 구조체로 레스터 라이저 상태 생성
    if (FAILED(device->CreateRasterizerState(&rasterDesc, &rasterState)))
    {
        return false;
    }

    // 레스터 상태 설정
    deviceContext->RSSetState(rasterState);

    // 뷰포트 설정
    D3D11_VIEWPORT viewPort;
    viewPort.Width = static_cast<float>(screenWidth);
    viewPort.Height = static_cast<float>(screenHeight);
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;
    viewPort.TopLeftX = 0.0f;
    viewPort.TopLeftY = 0.0f;

    // 뷰포트 생성
    deviceContext->RSSetViewports(1, &viewPort);

    // 투영 행렬 설정
    float fieldOfView = 3.141592654 / 4.0f;
    float screenAspect = static_cast<float>(screenWidth) / static_cast<float>(screenHeight);

    // 3D 렌더링을 위한 투영행렬 생성
    projectionMatrix = XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

    // 세계 행렬을 항등 행렬로 초기화
    worldMatrix = XMMatrixIdentity();

    // 2D 렌더링을 위한 직교 투영 행렬 생성
    orthoMatrix = XMMatrixOrthographicLH(static_cast<float>(screenWidth), static_cast<float>(screenHeight), screenNear, screenDepth);

    return true;
}

void D3DClass::Shutdown()
{
    if (swapChain)
    {
        swapChain->SetFullscreenState(false, nullptr);
    }

    if (rasterState)
    {
        rasterState->Release();
        rasterState = nullptr;
    }

    if (depthStencilView)
    {
        depthStencilView->Release();
        depthStencilView = nullptr;
    }

    if (depthStencilState)
    {
        depthStencilState->Release();
        depthStencilState = nullptr;
    }

    if (depthStencilBuffer)
    {
        depthStencilBuffer->Release();
        depthStencilBuffer = nullptr;
    }

    if (renderTargetView)
    {
        renderTargetView->Release();
        renderTargetView = nullptr;
    }

    if (deviceContext)
    {
        deviceContext->Release();
        deviceContext = nullptr;
    }

    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (swapChain)
    {
        swapChain->Release();
        swapChain = nullptr;
    }
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
    // 버퍼를 지울 색 설정
    float color[4] = { red, green, blue, alpha };

    // 백버퍼 지움
    deviceContext->ClearRenderTargetView(renderTargetView, color);

    // 깊이 버퍼 지움
    deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3DClass::EndScene()
{
    // 렌더링 완료후 화면에 백 버퍼 표시
    if (vsyncEnabled)
    {
        swapChain->Present(1, 0);
    }
    else
    {
        // 가능한 빠르게 출력
        swapChain->Present(0, 0);
    }
}

ID3D11Device* D3DClass::GetDevice()
{
    return device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
    return deviceContext;
}

void D3DClass::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = this->projectionMatrix;
    
}

void D3DClass::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = this->worldMatrix;
}

void D3DClass::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = this->orthoMatrix;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, videoCardDescription);
    memory = videoCardMemory;
}
