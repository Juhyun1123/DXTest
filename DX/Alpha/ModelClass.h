#pragma once

#ifndef _MODELCLASS_H_
#define _MODELCLASS_H_

class TextureClass;

class ModelClass
{

private:
	struct VertexType
	{
		XMFLOAT3 position;
		//XMFLOAT4 color; // 색
		XMFLOAT2 texture; // 텍스처 
	};

public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Init(ID3D11Device* device, const WCHAR* textureFilename);
	void Shutdown();
	void Render(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitBuffers(ID3D11Device* device);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext* deviceContext);

	bool LoadTexture(ID3D11Device* device, const WCHAR* filename);
	void ReleaseTexture();

private:
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	int vertexCount = 0;
	int indexCount = 0;

	TextureClass* texture = nullptr;
};

#endif
