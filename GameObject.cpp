#include "GameObject.h"


GameObject::GameObject(void)
{
}


GameObject::~GameObject(void)
{
}

void GameObject::Init(MeshData meshData)
{
	_meshData = meshData;

	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMStoreFloat4x4(&_scale, XMMatrixIdentity());
	XMStoreFloat4x4(&_rotate, XMMatrixIdentity());
	XMStoreFloat4x4(&_translate, XMMatrixIdentity());
	XMStoreFloat4x4(&_yaw, XMMatrixIdentity());
}

void GameObject::StartDir(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	mRight = pos;
	mLook = target;
	mUp = up;
}

void GameObject::SetScale(float x, float y, float z)
{
	XMStoreFloat4x4(&_scale, XMMatrixScaling(x, y, z));
}

void GameObject::SetRotation(float x, float y, float z)
{
	XMStoreFloat4x4(&_rotate, XMMatrixRotationX(x) * XMMatrixRotationY(y) * XMMatrixRotationZ(z) );

	currentRotationY = _rotate._42 + y;
	currentRotationX = _rotate._41 + x;
}

float GameObject::getRotationX()
{
	return currentRotationX;
}

float GameObject::getRotationY()
{
	return currentRotationY;
}

void GameObject::SetTranslation(float x, float y, float z)
{
	XMStoreFloat4x4(&_translate, XMMatrixTranslation(x, y, z));
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void GameObject::SetYaw(float yaw)
{

	XMStoreFloat4x4(&_yaw, XMMatrixRotationY(yaw));
}

void GameObject::ForwardBackward(float d)
{
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&mLook);
	XMVECTOR p = XMLoadFloat3(&mPosition);
	XMStoreFloat3(&mPosition, XMVectorMultiplyAdd(s, l, p));

	XMVECTOR finalPos = XMLoadFloat3(&mPosition);

	XMStoreFloat4x4(&_translate, XMMatrixTranslationFromVector(finalPos));

	//mPosition.z = mPosition.z += d;
}

void GameObject::LeftRight(float angle)
{
	XMMATRIX R = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), R));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

void GameObject::UpDown(float angle)
{
	XMMATRIX R = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), R));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), R));
}

XMFLOAT3 GameObject::GetPosition()const
{
	return mPosition;
}

float GameObject::GetLookZ()
{
	return mLook.z;
}

void GameObject::UpdateWorld()
{
	XMMATRIX scale = XMLoadFloat4x4(&_scale);
	XMMATRIX rotate = XMLoadFloat4x4(&_rotate);
	XMMATRIX translate = XMLoadFloat4x4(&_translate);
	XMMATRIX yaw = XMLoadFloat4x4(&_yaw);


	//mPosition;

	XMStoreFloat4x4(&_world, scale * rotate * translate * yaw);
}

void GameObject::UpdateWorldPlane()
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

	_world(0, 0) = mRight.x;
	_world(1, 0) = mRight.y;
	_world(2, 0) = mRight.z;
	_world(3, 0) = x;

	_world(0, 1) = mUp.x;
	_world(1, 1) = mUp.y;
	_world(2, 1) = mUp.z;
	_world(3, 1) = y;

	_world(0, 2) = mLook.x;
	_world(1, 2) = mLook.y;
	_world(2, 2) = mLook.z;
	_world(3, 2) = z;

	_world(0, 3) = 0.0f;
	_world(1, 3) = 0.0f;
	_world(2, 3) = 0.0f;
	_world(3, 3) = 1.0f;
}

void GameObject::Update(float elapsedTime)
{
	UpdateWorld();
}

void GameObject::Draw(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext)
{
	pImmediateContext->IASetVertexBuffers(0, 1, &_meshData.VertexBuffer, &_meshData.VBStride, &_meshData.VBOffset);
	pImmediateContext->IASetIndexBuffer(_meshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->DrawIndexed(_meshData.IndexCount, 0, 0);
}