#include "Camera.h"


Camera::Camera()
{

}


Camera::~Camera()
{
}


XMVECTOR Camera::GetPositionXM()const
{
	return XMLoadFloat3(&mPosition);
}

XMFLOAT3 Camera::GetPosition()const
{
	return mPosition;
}

void Camera::SetPosition(float x, float y, float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void Camera::SetPosition(const XMFLOAT3& v)
{
	mPosition.x = v.x;
	mPosition.y = v.y;
	mPosition.z = v.z;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	mFovY = fovY;
	mAspect = aspect;

	mNearZ = zn;
	mFarZ = zf;

	mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f*mFovY);
	mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f*mFovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
	XMStoreFloat4x4(&mProjection, P);
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	mRight = pos;
	mLook = target;
	mUp = up;

	XMVECTOR Eye = XMLoadFloat3(&mRight);
	XMVECTOR At = XMLoadFloat3(&mLook);
	XMVECTOR Up = XMLoadFloat3(&mUp);
	
	XMStoreFloat4x4(&mViewAt, XMMatrixLookAtLH(Eye, At, Up));
}

XMFLOAT3 Camera::GetLook()const
{
	return mLook;
}

float Camera::GetFovX()const
{
	float halfWidth = 0.5f*GetNearWindowWidth();
	return 2.0f*atan(halfWidth / mNearZ);
}

float Camera::GetNearWindowWidth()const
{
	return mAspect * mNearWindowHeight;
}

float Camera::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float Camera::GetFarWindowWidth()const
{
	return mAspect*mFarWindowHeight;
}

float Camera::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void Camera::Walk(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));
}

void Camera::Strafe(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR r = XMLoadFloat3(&mRight);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, r, p));
}


void Camera::Pitch(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void Camera::RotateY(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}


XMMATRIX Camera::View()const
{
	XMMATRIX view = XMLoadFloat4x4(&mViewAt);
	return view;
}

XMFLOAT4X4 Camera::ViewFL()const
{
	return mViewAt;
}

XMMATRIX Camera::Proj()const
{
	XMMATRIX proj = XMLoadFloat4x4(&mProjection);
	return proj;
}

XMFLOAT4X4 Camera::ProjFL()const
{
	return mProjection;
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR R = XMLoadFloat3(&mRight);
	XMVECTOR U = XMLoadFloat3(&mUp);
	XMVECTOR L = XMLoadFloat3(&mLook);
	XMVECTOR P = XMLoadFloat3(&mPosition);

	L = XMVector3Normalize(L); // Normalise the Look vector
	U = XMVector3Normalize(XMVector3Cross(L, R)); // Normalise the Up vector
	R = XMVector3Cross(U, L); // Normalise the Right vector

	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&mRight, R);
	XMStoreFloat3(&mUp, U);
	XMStoreFloat3(&mLook, L);

	mViewAt(0, 0) = mRight.x;
	mViewAt(1, 0) = mRight.y;
	mViewAt(2, 0) = mRight.z;
	mViewAt(3, 0) = x;

	mViewAt(0, 1) = mUp.x;
	mViewAt(1, 1) = mUp.y;
	mViewAt(2, 1) = mUp.z;
	mViewAt(3, 1) = y;

	mViewAt(0, 2) = mLook.x;
	mViewAt(1, 2) = mLook.y;
	mViewAt(2, 2) = mLook.z;
	mViewAt(3, 2) = z;

	mViewAt(0, 3) = 0.0f;
	mViewAt(1, 3) = 0.0f;
	mViewAt(2, 3) = 0.0f;
	mViewAt(3, 3) = 1.0f;
}