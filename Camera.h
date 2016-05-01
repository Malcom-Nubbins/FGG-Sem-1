#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "resource.h"

using namespace DirectX;

class Camera
{
private:
	int _WindowWidth;
	int _WindowHeight;

	XMFLOAT3 mPosition;
	XMFLOAT3 mRight; // x-axis
	XMFLOAT3 mUp; // y-axis
	XMFLOAT3 mLook; // z-axis

	float mNearZ;
	float mFarZ;
	float mAspect;
	float mFovY;
	float mNearWindowHeight;
	float mFarWindowHeight;

	XMFLOAT4X4 mViewAt;
	XMFLOAT4X4 mProjection;

public:
	Camera();
	~Camera();

	XMVECTOR GetPositionXM()const;
	XMFLOAT3 GetPosition()const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& v);

	XMVECTOR GetRightXM()const;
	XMFLOAT3 GetRight()const;
	XMVECTOR GetUpXM()const;
	XMFLOAT3 GetUp()const;
	XMVECTOR GetLookXM()const;
	XMFLOAT3 GetLook()const;

	float GetNearZ()const;
	float GetFarZ()const;
	float GetAspect()const;
	float GetFovY()const;
	float GetFovX()const;

	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	void SetLens(float fovY, float aspect, float zn, float zf);

	void LookAtXM(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up); // Right, Look, Up?


	XMMATRIX View()const;
	XMFLOAT4X4 ViewFL()const;
	XMMATRIX Proj()const;
	XMFLOAT4X4 ProjFL()const;

	void Strafe(float d);
	void Walk(float d);

	void Pitch(float angle);
	void RotateY(float angle);
	void Roll(float angle);

	void UpdateViewMatrix();

	float CamEyeX, CamEyeY, CamEyeZ;
	float CamAtX, CamAtY, CamAtZ;

};

