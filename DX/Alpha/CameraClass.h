#pragma once

#ifndef __CAMERACLASS_H_
#define __CAMERACLASS_H_

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass& other);
	~CameraClass();

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	void GetViewMatrix(XMMATRIX& viewMatrix);

private:
	XMFLOAT3 position;
	XMFLOAT3 rotation;
	XMMATRIX viewMatrix;
};

#endif