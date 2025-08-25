#include "stdafx.h"
#include "ModelClass.h"
#include "TextureClass.h"

ModelClass::ModelClass()
    : vertexBuffer(nullptr)
    , indexBuffer(nullptr)
    , vertexCount(0)
    , indexCount(0)
{
}

ModelClass::ModelClass(const ModelClass&)
{
}

ModelClass::~ModelClass()
{
}

bool ModelClass::Init(ID3D11Device* device, const WCHAR* textureFilename)
{
    // 정점과 인덱스 버퍼 초기화
    if (!InitBuffers(device))
    {
        return false;
    }

    return LoadTexture(device, textureFilename);
}

void ModelClass::Shutdown()
{
    // 모델 텍스처 반환
    ReleaseTexture();
    
    // 버텍스 및 인덱스 버퍼 종료
    ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    // 그리기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼 셋
    RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
    return indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
    return texture->GetTexture();
}

bool ModelClass::InitBuffers(ID3D11Device* device)
{
    // 정점 배열의 정점 수 설정
    vertexCount = 4; 

    // 인덱스 배열의 인덱스 수 설정
    indexCount = 6;

    // 정점 배열 생성
    VertexType* vertices = new VertexType[vertexCount];
    if (!vertices)
    {
        return false;
    }

    // 인덱스 배열 생성
    unsigned long* indices = new unsigned long[indexCount];
    if (!indices)
    {
        return false;
    }
    
    // 시계방향으로 그려야 됨
    //vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f); // LT
    //vertices[0].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // yellow

    //vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f); // RT
    //vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // red

    //vertices[2].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // LB
    //vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f); // green

    //vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // RB
    //vertices[3].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // blue
    
       // 시계방향으로 그려야 됨
    vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f); // LT
    vertices[0].texture = XMFLOAT2(0.0f, 0.0f);

    vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f); // RT
    vertices[1].texture = XMFLOAT2(1.0f, 0.0f);

    vertices[2].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // LB
    vertices[2].texture = XMFLOAT2(0.0f, 1.0f);

    vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // RB
    vertices[3].texture = XMFLOAT2(1.0f, 1.0f);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

    indices[3] = 2;
    indices[4] = 1;
    indices[5] = 3;

    // 정점 버퍼의 description (서술자) 작성
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // 정점 데이터를 가리키는 보조 리소스 구조체 작성
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // 정점 버퍼 생성
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer)))
    {
        return false;
    }

    // 인덱스 버퍼 description 생성
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // 인덱스 데이터를 가리키는 보조 리소스 구조체 작성
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // 인덱스 버퍼 생성
    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer)))
    {
        return false;
    }

    // 정점 버퍼 인덱스 버퍼 해제 
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void ModelClass::ShutdownBuffers()
{
    // 인덱스 버퍼 해제
    if (indexBuffer)
    {
        indexBuffer->Release();
        indexBuffer = nullptr;
    }

    // 정점 버퍼 해제
    if (vertexBuffer)
    {
        vertexBuffer->Release();
        vertexBuffer = nullptr;
    }
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
    unsigned int stride;
    unsigned int offset;

    // 정점 버퍼의 단위와 오프셋 설정
    stride = sizeof(VertexType);
    offset = 0;

    // input Assembler에 정점 버퍼를 활성화한다.
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // input Assembler에 인덱스 버퍼를 활성화한다.
    deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 정점 버퍼를 그릴 기본 도형 선택 (삼각형)
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTexture(ID3D11Device* device, const WCHAR* filename)
{
    texture = new TextureClass;

    if (!texture)
    {
        return false;
    }

    return texture->Init(device, filename);
}

void ModelClass::ReleaseTexture()
{
    if (texture)
    {
        texture->Shutdown();
        delete texture;
        texture = nullptr;
    }
}
