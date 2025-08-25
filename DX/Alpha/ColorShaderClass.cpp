﻿#include "stdafx.h"
#include "ColorShaderClass.h"
#include <fstream>
#include <iostream>
#include "TextureShaderClass.h"

ColorShaderClass::ColorShaderClass()
{
}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{
}

ColorShaderClass::~ColorShaderClass()
{
}


bool ColorShaderClass::Init(ID3D11Device* device, HWND hwnd)
{
    // 정점 쉐이더와 픽셀 쉐이더 초기화
    return InitShader(device, hwnd, L"./Color.vs.hlsl", L"./Color.ps.hlsl");
}

void ColorShaderClass::Shutdown()
{
    ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    // 렌더링에 사용할 쉐이더 인자 입력
    if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix))
    {
        return false;
    }

    // 쉐이더를 이용해 준비된 버퍼 그리기
    RenderShader(deviceContext, indexCount);
    return true;
}

bool ColorShaderClass::InitShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    ID3D10Blob* errorMessage = nullptr;

    // 버텍스 쉐이더 코드 컴파일
    ID3D10Blob* vertexShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(vsFilename, nullptr, nullptr, "ColorVertexShader", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShaderBuffer, &errorMessage)))
    {
        // 컴파일 실패시 오류 메세지 출력
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        else  // 컴파일 오류가 아닌경우 -> 파일 검색 실패
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }
    
    // 픽셀 쉐이더 코드 컴파일
    ID3D10Blob* pixelShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(psFilename, nullptr, nullptr, "ColorPixelShader", "ps_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
    {
        // 컴파일 실패시 오류 메세지 출력
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        else  // 컴파일 오류가 아닌경우 -> 파일 검색 실패
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // 버퍼로 부터 정점 쉐이더 생성
    if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
        nullptr, &vertexShader)))
    {
        return false;
    }

    // 버퍼로 부터 픽셀 쉐이더 생성
    if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
        nullptr, &pixelShader)))
    {
        return false;
    }

    // 정점 입력 레이아웃 구조체 설정
    // 이 설정은 ModelClass와 쉐이더의 VertexType 구조와 일치해야함
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "COLOR";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // 레이아웃의 요소 수를 가져옵니다.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // 정점 입력 레이아웃 생성
    if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &layout)))
    {
        return false;
    }

    // 더 이상 사용되지 않는 정점 쉐이더 버퍼와 픽셀 쉐이더 버퍼 해제
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    // 정점 쉐이더에 있는 행렬 상수 버퍼 구조체 작성
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // 상수 버퍼 포인터를 만들어 이 클래스에서 정점 쉐이더 상수에 접근 가능하게 한다.
    if (FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &matrixBuffer)))
    {
        return false;
    }

    return true;
}

void ColorShaderClass::ShutdownShader()
{
    if (matrixBuffer)
    {
        matrixBuffer->Release();
        matrixBuffer = nullptr;
    }

    if (layout)
    {
        layout->Release();
        layout = nullptr;
    }

    if (pixelShader)
    {
        pixelShader->Release();
        pixelShader = nullptr;
    }

    if (vertexShader)
    {
        vertexShader->Release();
        vertexShader = nullptr;
    }
}

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    OutputDebugStringA(reinterpret_cast<char*>(errorMessage->GetBufferPointer()));

    //////////////////////////////////
    // 파일로 에러출력.

    char* compileErrors;
    unsigned long bufferSize, i;
    std::ofstream fout;


    //// 에러 메세지를 담고 있는 문자열 버퍼의 포인터를 가져옵니다.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    //// 메세지의 길이를 가져옵니다.
    bufferSize = errorMessage->GetBufferSize();

    //// 파일을 열고 안에 메세지를 기록합니다.
    fout.open("shader-error.txt");

    //// 에러 메세지를 씁니다.
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    //// 파일을 닫습니다.
    fout.close();

    /////////////////////////////////////
    /////////////////////////////////////


    // 에러 메시지를 반환 한다.
    errorMessage->Release();
    errorMessage = nullptr;

    // 컴파일 에러가 있음을 팝업 메세지를 알려준다.
    MessageBox(hwnd, L"Error compiling shader.", shaderFilename, MB_OK);
}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
{
    // 행렬을 transpose하여 쉐이더에서 사용할 수 있게 한다.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // 상수 버퍼의 내용을 쓸 수 있도록 잠근다.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // 상수 버퍼의 데이터에 대한 포인터 획득
    MatrixBufferType* dataPtr = reinterpret_cast<MatrixBufferType*>(mappedResource.pData);

    // 상수 버퍼에 행렬 복사
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // 상수 버퍼 잠금 해제
    deviceContext->Unmap(matrixBuffer, 0);

    // 정점 쉐이더에서의 상수 버퍼의 위치 설정
    unsigned bufferNumber = 0;

    // 마지막으로 정점 쉐이더의 상수 버퍼를 바뀐 값으로 변경
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

    return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    //정점 입력 레이아웃 설정
    deviceContext->IASetInputLayout(layout);

    // 삼각형을 그릴 정점 쉐이더와 픽셀 쉐이더 설정
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    // 삼각형 그리기
    deviceContext->DrawIndexed(indexCount, 0, 0);
}
