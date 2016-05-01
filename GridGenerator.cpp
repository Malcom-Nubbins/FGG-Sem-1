#include "GridGenerator.h"


GridGenerator::GridGenerator()
{
}


GridGenerator::~GridGenerator()
{
}

void GridGenerator::LoadHeightMap()
{
	mInfo.heightScale = 80;
	mInfo.heightMapFileName = (L"textures/terrainHeightMap.raw");
	mInfo.heightMapHeight = 512;
	mInfo.heightMapWidth = 512;

	std::vector<unsigned char> in(mInfo.heightMapWidth * mInfo.heightMapHeight);

	std::ifstream inFile;
	inFile.open(mInfo.heightMapFileName.c_str(), std::ios_base::binary);

	if (inFile)
	{
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		inFile.close();
	}

	heightmap.resize(mInfo.heightMapHeight * mInfo.heightMapWidth, 0);

	for (UINT i = 0; i < mInfo.heightMapHeight * mInfo.heightMapWidth; i++)
	{
		heightmap[i] = (in[i] / 255.0f) * mInfo.heightScale;
	}
}

void GridGenerator::CreateGrid(float width, float depth, UINT m, UINT n, GridMeshData& gridMeshData, bool useHeightMap)
{
	heightMapUse = useHeightMap;
	LoadHeightMap();
	UINT vertexCount = m*n;
	UINT faceCount = (m - 1)*(n - 1) * 2;

	//Create Vertices

	float halfWidth = 0.5*width;
	float halfDepth = 0.5*depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	gridMeshData.Vertices.resize(vertexCount);
	gridMeshData.Indices.resize(faceCount * 3);

	
	if (heightMapUse == true)
	{
		for (UINT i = 0; i < m; i++)
		{
			float z = halfDepth - i*dz;

			for (UINT j = 0; j < n; j++)
			{
				float x = -halfWidth + j*dx;

				gridMeshData.Vertices[i*n + j].Position = XMFLOAT3(x, heightmap[j*n + i], z);
				gridMeshData.Vertices[i*n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				gridMeshData.Vertices[i*n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

				gridMeshData.Vertices[i*n + j].TexC.x = j*du;
				gridMeshData.Vertices[i*n + j].TexC.y = i*dv;
			}
		}
	}
	else
	{
		for (UINT i = 0; i < m; i++)
		{
			float z = halfDepth - i*dz;

			for (UINT j = 0; j < n; j++)
			{
				float x = -halfWidth + j*dx;

				gridMeshData.Vertices[i*n + j].Position = XMFLOAT3(x, 0.0f, z);
				gridMeshData.Vertices[i*n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
				gridMeshData.Vertices[i*n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

				gridMeshData.Vertices[i*n + j].TexC.x = j*du;
				gridMeshData.Vertices[i*n + j].TexC.y = i*dv;
			}
		}
	}





	UINT k = 0;
	for (UINT i = 0; i < m - 1; i++)
	{
		for (UINT j = 0; j < n - 1; j++)
		{
			gridMeshData.Indices[k] = i*n + j;
			gridMeshData.Indices[k+1] = i*n + j + 1;
			gridMeshData.Indices[k+2] = (i+1)*n + j;
			gridMeshData.Indices[k+3] = (i+1)*n + j;
			gridMeshData.Indices[k+4] = i*n + j + 1;
			gridMeshData.Indices[k+5] = (i+1)*n + j+1;
			k += 6;
		}
	}
}



