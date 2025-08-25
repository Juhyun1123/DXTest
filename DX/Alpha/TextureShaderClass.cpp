#include "stdafx.h"
#include "TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{

}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{

}

TextureShaderClass::~TextureShaderClass()
{

}

bool TextureShaderClass::Init(ID3D11Device* device, HWND hwnd)
{
	// ���� �� �ȼ� ���̴� �ʱ�ȭ
	return InitShader(device, hwnd, L"./Texture.vs.hlsl", L"./Texture.ps.hlsl");
}

void TextureShaderClass::Shutdown()
{
	// ���ؽ� �� �ȼ� ���̴��� ���õ� ��ü ����
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
	// �������� ����� ���̴� �Ű� ���� ����
	if (!SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture))
	{
		return false;
	}

	// ������ ���۸� ���̴��� ������
	RenderShader(deviceContext, indexCount);
	return true;
}


bool TextureShaderClass::InitShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
{
    ID3D10Blob* errorMessage = nullptr;

    ID3D10Blob* vertexShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(vsFilename, nullptr, nullptr, "TextureVertexShader", "vs_5_0",
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &vertexShaderBuffer, &errorMessage)))
    {
        // ������ ���н� ���� �޼��� ���
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        else  // ������ ������ �ƴѰ�� -> ���� �˻� ����
        {
            MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // �ȼ� ���̴� �ڵ� ������
    ID3D10Blob* pixelShaderBuffer = nullptr;
    if (FAILED(D3DCompileFromFile(psFilename, nullptr, nullptr, "TexturePixelShader", "ps_5_0",
        D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
    {
        // ������ ���н� ���� �޼��� ���
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        else  // ������ ������ �ƴѰ�� -> ���� �˻� ����
        {
            MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
        }

        return false;
    }

    // ���۷� ���� ���� ���̴� ����
    if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(),
        nullptr, &vertexShader)))
    {
        return false;
    }

    // ���۷� ���� �ȼ� ���̴� ����
    if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(),
        nullptr, &pixelShader)))
    {
        return false;
    }

    // ���� �Է� ���̾ƿ� ����ü ����
    // �� ������ ModelClass�� ���̴��� VertexType ������ ��ġ�ؾ���
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    //?���̾ƿ���?���?����?�����ɴϴ�.
    unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // ���� �Է� ���̾ƿ� ����
    if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &layout)))
    {
        return false;
    }

    // �� �̻� ������ �ʴ� ���� ���̴� ���ۿ� �ȼ� ���̴� ���� ����
    vertexShaderBuffer->Release();
    vertexShaderBuffer = nullptr;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = nullptr;

    // ���� ���̴��� �ִ� ��� ��� ���� ����ü �ۼ�
    D3D11_BUFFER_DESC matrixBufferDesc;
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ����� ���� �����ϰ� �Ѵ�.
    if (FAILED(device->CreateBuffer(&matrixBufferDesc, nullptr, &matrixBuffer)))
    {
        return false;
    }

    // �ؽ�ó ���÷� ���� ����ü�� ���� �� ����
    D3D11_SAMPLER_DESC samplerDesc;
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // �ؽ�ó ���÷� ���¸� �����.
    if (FAILED(device->CreateSamplerState(&samplerDesc, &sampleState)))
    {
        return false;
    }

    return true;
}

void TextureShaderClass::ShutdownShader()
{
    if (sampleState)
    {
        sampleState->Release();
        sampleState = nullptr;

    }

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

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
{
    OutputDebugStringA(reinterpret_cast<char*>(errorMessage->GetBufferPointer()));

    //////////////////////////////////
    //?���Ϸ�?�������.
    /*
    char* compileErrors;
    unsigned long bufferSize, i;
    std::ofstream fout;


    ////?����?�޼�����?���?�ִ�?���ڿ�?������?�����͸�?�����ɴϴ�.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    ////?�޼�����?���̸�?�����ɴϴ�.
    bufferSize = errorMessage->GetBufferSize();

    ////?������?����?�ȿ�?�޼�����?����մϴ�.
    fout.open("shader-error.txt");

    ////?����?�޼�����?���ϴ�.
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    ////?������?�ݽ��ϴ�.
    fout.close();
    */
    /////////////////////////////////////
    /////////////////////////////////////


    //?����?�޽�����?��ȯ?�Ѵ�.
    errorMessage->Release();
    errorMessage = nullptr;

    //?������?������?������?�˾�?�޼�����?�˷��ش�.
    MessageBox(hwnd, L"Error?compiling?shader.", shaderFilename, MB_OK);
}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* texture)
{
    // ����� transpose �Ͽ� ���̴����� ����� �� �ְ� �Ѵ�.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // ��� ������ ������ �� �� �ֵ��� ���
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
    {
        return false;
    }

    // ��� ������ �����Ϳ� ���� �����͸� �����´�.
    MatrixBufferType* dataPtr = reinterpret_cast<MatrixBufferType*>(mappedResource.pData);

    // ��� ���ۿ� ����� �����Ѵ�.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // ��� ������ ��� ����
    deviceContext->Unmap(matrixBuffer, 0);

    // ���� ���̴������� ��� ���� ��ġ ����
    unsigned bufferNumber = 0;

    //���������� ���� ���̴��� ��� ���۸� �ٲ� ���� ����
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

    // �ȼ� ���̴����� ���̴� �ؽ�ó ���ҽ� ����
    deviceContext->PSSetShaderResources(0, 1, &texture);

    return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
    // ���� �Է� ���̾ƿ� ����
    deviceContext->IASetInputLayout(layout);

    // �ﰢ���� �׸� ���� ���̴��� �ȼ� ���̴��� ����
    deviceContext->VSSetShader(vertexShader, nullptr, 0);
    deviceContext->PSSetShader(pixelShader, nullptr, 0);

    // �ȼ� ���̴����� ���÷� ���� ����
    deviceContext->PSSetSamplers(0, 1, &sampleState);

    //�׸���
    deviceContext->DrawIndexed(indexCount, 0, 0);
}