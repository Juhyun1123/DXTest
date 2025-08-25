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
    // ������ �ε��� ���� �ʱ�ȭ
    if (!InitBuffers(device))
    {
        return false;
    }

    return LoadTexture(device, textureFilename);
}

void ModelClass::Shutdown()
{
    // �� �ؽ�ó ��ȯ
    ReleaseTexture();
    
    // ���ؽ� �� �ε��� ���� ����
    ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
    // �׸��� ���� �׷��� ������ ���ο� �������� �ε��� ���� ��
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
    // ���� �迭�� ���� �� ����
    vertexCount = 4; 

    // �ε��� �迭�� �ε��� �� ����
    indexCount = 6;

    // ���� �迭 ����
    VertexType* vertices = new VertexType[vertexCount];
    if (!vertices)
    {
        return false;
    }

    // �ε��� �迭 ����
    unsigned long* indices = new unsigned long[indexCount];
    if (!indices)
    {
        return false;
    }
    
    // �ð�������� �׷��� ��
    //vertices[0].position = XMFLOAT3(-1.0f, 1.0f, 0.0f); // LT
    //vertices[0].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // yellow

    //vertices[1].position = XMFLOAT3(1.0f, 1.0f, 0.0f); // RT
    //vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // red

    //vertices[2].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // LB
    //vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f); // green

    //vertices[3].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // RB
    //vertices[3].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // blue
    
       // �ð�������� �׷��� ��
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

    // ���� ������ description (������) �ۼ�
    D3D11_BUFFER_DESC vertexBufferDesc;
    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;
    vertexBufferDesc.StructureByteStride = 0;

    // ���� �����͸� ����Ű�� ���� ���ҽ� ����ü �ۼ�
    D3D11_SUBRESOURCE_DATA vertexData;
    vertexData.pSysMem = vertices;
    vertexData.SysMemPitch = 0;
    vertexData.SysMemSlicePitch = 0;

    // ���� ���� ����
    if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer)))
    {
        return false;
    }

    // �ε��� ���� description ����
    D3D11_BUFFER_DESC indexBufferDesc;
    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;
    indexBufferDesc.StructureByteStride = 0;

    // �ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü �ۼ�
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // �ε��� ���� ����
    if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer)))
    {
        return false;
    }

    // ���� ���� �ε��� ���� ���� 
    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void ModelClass::ShutdownBuffers()
{
    // �ε��� ���� ����
    if (indexBuffer)
    {
        indexBuffer->Release();
        indexBuffer = nullptr;
    }

    // ���� ���� ����
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

    // ���� ������ ������ ������ ����
    stride = sizeof(VertexType);
    offset = 0;

    // input Assembler�� ���� ���۸� Ȱ��ȭ�Ѵ�.
    deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // input Assembler�� �ε��� ���۸� Ȱ��ȭ�Ѵ�.
    deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // ���� ���۸� �׸� �⺻ ���� ���� (�ﰢ��)
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
