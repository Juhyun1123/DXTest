#include "stdafx.h"
#include "CameraClass.h"

CameraClass::CameraClass()
{
    position = XMFLOAT3(0.0f, 0.0f, 0.0f);
    rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CameraClass::CameraClass(const CameraClass& other)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
    position.x = x;
    position.y = y;
    position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
    rotation.x = x;
    rotation.y = y;
    rotation.z = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
    return position;
}

XMFLOAT3 CameraClass::GetRotation()
{
    return rotation;
}

void CameraClass::Render()
{
    XMFLOAT3 up, pos, lookAt;
    XMVECTOR upVector, positionVector, lookAtVector;
    float yaw, pitch, roll;
    XMMATRIX rotationMatrix;

    // ������ ����Ű�� ���� ����
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    // XMVECTOR ����ü�� �ε�
    upVector = XMLoadFloat3(&up);

    // 3D ���忡�� ī�޶� ��ġ ����
    pos = position;

    // XMVECTOR ����ü �ε�
    positionVector = XMLoadFloat3(&pos);

    // �⺻������ ī�޶� �ٶ󺸴� ��ġ ����
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

    // XMVECTOR ����ü�� �ε�
    lookAtVector = XMLoadFloat3(&lookAt);

    // yaw(y��), pitch(x��), �׸��� roll(z��)�� ȸ������ ���� ������ ����
    pitch = rotation.x * 0.0174532925f;
    yaw = rotation.y * 0.0174532925f;
    roll = rotation.z * 0.0174532925f;

    // yaw, pitch, rool ���� ���� ȸ�� ��� ����
    rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // lookAt �� up ���͸� ȸ�� ��ķ� ������ �䰡 �������� �ùٸ��� ȸ���ǵ��� ����
    lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
    upVector = XMVector3TransformCoord(upVector, rotationMatrix);

    // ȸ�� �� ī�޶� ��ġ�� ��� ��ġ�� ��ȯ
    lookAtVector = XMVectorAdd(positionVector, lookAtVector);

    // ���������� �� ���� ������Ʈ �� ���Ϳ��� �� ��� ����
    viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = this->viewMatrix;
}