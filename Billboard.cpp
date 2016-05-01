#include "Billboard.h"


Billboard::Billboard(void)
{

}


Billboard::~Billboard(void)
{
}

void Billboard::Init()
{
	XMStoreFloat4x4(&_world, XMMatrixIdentity());
	XMStoreFloat4x4(&_scale, XMMatrixIdentity());
	XMStoreFloat4x4(&_translate, XMMatrixIdentity());

}

BillboardMesh Billboard::CreateBBQuad(ID3D11Device* _pd3dDevice)
{
	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(1.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -2.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }
	};

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = vertices;

	_pd3dDevice->CreateBuffer(&bd, &InitData, &mesh.VertexBuffer);

	mesh.VBOffset = 0;
	mesh.VBStride = sizeof(SimpleVertex);

	WORD indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(WORD) * 6;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA iInitData;
	ZeroMemory(&iInitData, sizeof(iInitData));
	iInitData.pSysMem = indices;
	_pd3dDevice->CreateBuffer(&ibd, &iInitData, &mesh.IndexBuffer);

	mesh.IndexCount = 6;

	return mesh;
}

void Billboard::SetScale(float x, float y, float z)
{
	XMStoreFloat4x4(&_scale, XMMatrixScaling(x, y, z));
}

void Billboard::SetTranslation(float x, float y, float z)
{
	XMStoreFloat4x4(&_translate, XMMatrixTranslation(x, y, z));
	position.x = x;
	position.y = y;
	position.z = z;
}

void Billboard::CalculateLookDirection(XMFLOAT3 eyePos)
{
	XMFLOAT3 quadCentrePoint = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 mEyePos = eyePos;
	XMFLOAT3 right = XMFLOAT3(_world._11, _world._21, _world._31);
	XMFLOAT3 pos = XMFLOAT3(_world._13, _world._23, _world._33);
	XMFLOAT3 look;
	look.x = (pos.x * 2) - mEyePos.x;
	look.y = 0.0f;
	look.z = (pos.z * 2) - mEyePos.z;

	XMVECTOR L = XMLoadFloat3(&look);
	L = XMVector3Normalize(L);

	up = XMFLOAT3(_world._12, _world._22, _world._32);
	XMVECTOR U = XMLoadFloat3(&up);


	XMVECTOR R = XMVector3Cross(L, U);
	//R = R *(0.5);

	//U = XMVector3Cross(L, R);

	XMStoreFloat3(&right, R);
	XMStoreFloat3(&up, U);
	XMStoreFloat3(&look, L);

	_world(0, 0) = right.x;
	_world(1, 0) = right.y;
	_world(2, 0) = right.z;
	_world(3, 0) = position.x;

	_world(0, 1) = up.x;
	_world(1, 1) = up.y;
	_world(2, 1) = up.z;
	_world(3, 1) = position.y;

	_world(0, 2) = look.x;
	_world(1, 2) = look.y;
	_world(2, 2) = look.z;
	_world(3, 2) = position.z;

	_world(0, 3) = 0.0;
	_world(1, 3) = 0.0;
	_world(2, 3) = 0.0;
	_world(3, 3) = 1.0f;

	
}

void Billboard::Update()
{
	XMMATRIX scale = XMLoadFloat4x4(&_scale);
	XMMATRIX translate = XMLoadFloat4x4(&_translate);

	XMStoreFloat4x4(&_world, scale * translate);
}

void Billboard::Draw(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext)
{
	pImmediateContext->IASetVertexBuffers(0, 1, &mesh.VertexBuffer, &mesh.VBStride, &mesh.VBOffset);
	pImmediateContext->IASetIndexBuffer(mesh.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	pImmediateContext->DrawIndexed(mesh.IndexCount, 0, 0);
}

