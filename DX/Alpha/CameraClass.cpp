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

    // 위쪽을 가리키는 벡터 설정
    up.x = 0.0f;
    up.y = 1.0f;
    up.z = 0.0f;

    // XMVECTOR 구조체에 로드
    upVector = XMLoadFloat3(&up);

    // 3D 월드에서 카메라 위치 설정
    pos = position;

    // XMVECTOR 구조체 로드
    positionVector = XMLoadFloat3(&pos);

    // 기본적으로 카메라 바라보는 위치 설정
    lookAt.x = 0.0f;
    lookAt.y = 0.0f;
    lookAt.z = 1.0f;

    // XMVECTOR 구조체에 로드
    lookAtVector = XMLoadFloat3(&lookAt);

    // yaw(y축), pitch(x축), 그리고 roll(z축)의 회전값을 라디안 단위로 설정
    pitch = rotation.x * 0.0174532925f;
    yaw = rotation.y * 0.0174532925f;
    roll = rotation.z * 0.0174532925f;

    // yaw, pitch, rool 값을 통해 회전 행렬 생성
    rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

    // lookAt 및 up 벡터를 회전 행렬로 변형해 뷰가 원점에서 올바르게 회전되도록 세팅
    lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
    upVector = XMVector3TransformCoord(upVector, rotationMatrix);

    // 회전 된 카메라 위치를 뷰어 위치로 변환
    lookAtVector = XMVectorAdd(positionVector, lookAtVector);

    // 마지막으로 세 개의 업데이트 된 벡터에서 뷰 행렬 생성
    viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);
}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
    viewMatrix = this->viewMatrix;
}