#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Structures.h"

using namespace DirectX;

struct BillboardMesh
{
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};

class Billboard
{

private:
	XMFLOAT4X4 _world;
	XMFLOAT4X4 _scale;
	XMFLOAT4X4 _translate;

	XMFLOAT3 up;
	XMFLOAT3 position;

	BillboardMesh mesh;

public:
	Billboard();
	~Billboard();
	void Init();

	XMFLOAT4X4 GetWorld() const { return _world; };

	BillboardMesh CreateBBQuad(ID3D11Device* _pd3dDevice);

	void SetScale(float x, float y, float z);
	void SetTranslation(float x, float y, float z);

	void Update();
	void CalculateLookDirection(XMFLOAT3 eyePos);
	void Draw(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext);
};

