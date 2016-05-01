#pragma once
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace DirectX;

class GridGenerator
{
public:
	GridGenerator();
	~GridGenerator();


	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT3 TangentU;
		XMFLOAT2 TexC;

		Vertex(){}
		Vertex(const XMFLOAT3& p, const XMFLOAT3& n, const XMFLOAT3& t, const XMFLOAT2& uv) : Position(p), Normal(n), TangentU(t), TexC(uv){}
		Vertex(
			float px, float py, float pz,
			float nx, float ny, float nz,
			float tx, float ty, float tz,
			float u, float v) : Position(px, py, pz), Normal(nx, ny, nz), TangentU(tx, ty, tz), TexC(u, v){}

	};

	struct GridMeshData
	{
		std::vector<Vertex> Vertices;
		std::vector<UINT> Indices;
	};

	struct InitInfo
	{
		std::wstring heightMapFileName;
		
		UINT heightMapHeight;
		UINT heightMapWidth;
		float heightScale;

		float CellSpacing;
	};

	void LoadHeightMap();
	void CreateGrid(float width, float depth, UINT m, UINT n, GridMeshData& gridMeshData, bool useHeightMap);


private:
	std::vector<float> heightmap;
	InitInfo	mInfo;
	bool heightMapUse;

};

