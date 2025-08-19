#include "stdafx.h"
#include "ModelClass.h"

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

bool ModelClass::Init(ID3D11Device* device)
{
    // ������ �ε��� ���� �ʱ�ȭ
    return InitBuffers(device);
}

void ModelClass::Shutdown()
{
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

bool ModelClass::InitBuffers(ID3D11Device* device)
{
    // ���� �迭�� ���� �� ����
    vertexCount = 3; 

    // �ε��� �迭�� �ε��� �� ����
    indexCount = 3;

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
    vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // left
    vertices[0].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); //blue

    vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // top
    vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // red �Ƹ���? rgb��

    vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // right
    vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f); // green


    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;

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
