#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "resource.h"
#include <fstream>
#include <string>
#include <iostream>

using namespace DirectX;

struct MeshData
{
	ID3D11Buffer * VertexBuffer;
	ID3D11Buffer * IndexBuffer;
	UINT VBStride;
	UINT VBOffset;
	UINT IndexCount;
};

class GameObject
{
private:
	MeshData _meshData;
	XMFLOAT4X4 _world;

	XMFLOAT4X4 _scale;
	XMFLOAT4X4 _rotate;
	XMFLOAT4X4 _translate;
	XMFLOAT4X4 _yaw;

	XMFLOAT3 mPosition;
	XMFLOAT3 mRight; // x-axis
	XMFLOAT3 mUp; // y-axis
	XMFLOAT3 mLook; // z-axis

	float currentRotationY;
	float currentRotationX;


public:
	GameObject(void);
	~GameObject(void);

	XMFLOAT4X4 GetWorld() const { return _world; };

	void UpdateWorld();
	void UpdateWorldPlane();

	void SetScale(float x, float y, float z);

	void SetRotation(float x, float y, float z);

	float getRotationX();
	float getRotationY();
	
	void SetYaw(float);
	void SetTranslation(float x, float y, float z);

	void ForwardBackward(float d);
	void LeftRight(float angle);
	void UpDown(float angle);

	XMFLOAT3 GetPosition()const;
	float GetLookZ();

	void StartDir(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	void Init(MeshData meshData);
	void Update(float elapsedTime);
	void Draw(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pImmediateContext);
};

