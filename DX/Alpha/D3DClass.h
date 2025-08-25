#pragma once

#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

class D3DClass
{
	
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Init(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullScreen, float screenDepth, float screenNear);
	void Shutdown();

	void BeginScene(float red, float green, float blue, float alpha);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX& projectionMatrix);
	void GetWorldMatrix(XMMATRIX& worldMatrix);
	void GetOrthoMatrix(XMMATRIX& orthoMatrix);

	void GetVideoCardInfo(char* cardName, int& memory);

private:
	bool vsyncEnabled = false;
	int videoCardMemory = 0;
	char videoCardDescription[128] = { 0 , };

	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView = nullptr;

	ID3D11Texture2D* depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* depthStencilState = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11RasterizerState* rasterState = nullptr;

	XMMATRIX projectionMatrix;
	XMMATRIX worldMatrix;
	XMMATRIX orthoMatrix;
};


#endif
