#pragma once

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <directxcolors.h>
#include "resource.h"
#include "Camera.h"
#include "DDSTextureLoader.h"
#include "GameObject.h"
#include "OBJLoader.h"
#include "GridGenerator.h"
#include "Structures.h"
#include <dinput.h>
#include <windowsx.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")

using namespace DirectX;



struct ConstantBuffer
{
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;
	XMFLOAT4 DiffuseMtrl;
	XMFLOAT4 DiffuseLight;
	XMFLOAT4 AmbientMtrl;
	XMFLOAT4 AmbientLight;
	XMFLOAT4 SpecularMtrl;
	XMFLOAT4 SpecularLight;
	float	SpecularPower;
	XMFLOAT3 pad;
	XMFLOAT3 EyePosW;
	float pad3;
	XMFLOAT3 LightVecW;
	float time;
	float gFogStart;
	float gFogRange;
	XMFLOAT2 pad2;
	XMFLOAT4 gFogColor;
	float gFogEnabled;
};



class Application
{
private:
	HINSTANCE               _hInst;
	HWND                    _hWnd;
	D3D_DRIVER_TYPE         _driverType;
	D3D_FEATURE_LEVEL       _featureLevel;
	ID3D11Device*           _pd3dDevice;
	ID3D11DeviceContext*    _pImmediateContext;
	IDXGISwapChain*         _pSwapChain;
	ID3D11RenderTargetView* _pRenderTargetView;
	ID3D11VertexShader*     _pVertexShader;
	ID3D11PixelShader*      _pPixelShader;
	ID3D11InputLayout*      _pVertexLayout;
	ID3D11Buffer*           _pVertexBuffer;
	ID3D11Buffer*			_pTriVertexBuffer;
	ID3D11Buffer*			_pGridVertexBuffer;
	ID3D11Buffer*			_pGrid3VertexBuffer;
	ID3D11Buffer*           _pIndexBuffer;
	ID3D11Buffer*			_pTriIndexBuffer;
	ID3D11Buffer*			_pGridIndexBuffer;
	ID3D11Buffer*			_pGrid3IndexBuffer;
	ID3D11Buffer*           _pConstantBuffer;
	XMFLOAT4X4				_world4;
	XMFLOAT4X4				_world5;
	XMFLOAT4X4				_world6[100];
	XMFLOAT4X4				_gridWorld;
	XMFLOAT4X4				_gridWorld3;
	XMFLOAT4X4				_skyBox;
	XMFLOAT4X4              _view;
	XMFLOAT4X4              _projection;
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11Texture2D*		_depthStencilBuffer;
	ID3D11RasterizerState*	_wireFrame;
	ID3D11RasterizerState*	_solidFill;
	ID3D11ShaderResourceView* _pTextureRV = nullptr;
	ID3D11ShaderResourceView* _pTreeTextureRV = nullptr;
	ID3D11ShaderResourceView* _grassTextureRV = nullptr;
	ID3D11ShaderResourceView* _planeTextureRV = nullptr;
	ID3D11ShaderResourceView* _oceanTextureRV = nullptr;
	ID3D11ShaderResourceView* _skyTextureRV = nullptr;
	ID3D11ShaderResourceView* _buildingTextureRV = nullptr;
	ID3D11SamplerState*		_pSamplerLinear = nullptr;
	ID3D11BlendState*		Transparency;

	float					time;

	XMFLOAT3				lightDirection;
	XMFLOAT4				diffuseMaterial;
	XMFLOAT4				diffuseLight;
	XMFLOAT4				ambientMaterial;
	XMFLOAT4				ambientLight;
	XMFLOAT4				specularMaterial;
	XMFLOAT4				specularLight;
	float					specularPower;
	float					gFogStart;
	float					gFogRange;
	XMFLOAT4				gFogColor;
	XMFLOAT3				eyePosition;

	Camera					_camera;
	Camera					_camera2;
	Camera					_camera3;
	Camera					_camera4;

	MeshData				objMeshData;
	MeshData				objMeshData2;
	MeshData				objMeshData3;
	MeshData				buildingMesh;
	GameObject*				objMesh;
	GameObject*				objMesh2;
	GameObject*				objMesh3;
	GameObject*				objMesh4;
	GameObject*				objMesh6[100];

	GameObject*				buildingObject[20];
	

	UINT mGridVertexCount;
	UINT mGrid3VertexCount;
	LONG mGridIndexCount;
	LONG mGrid3IndexCount;
	GridGenerator::GridMeshData	grid;
	GridGenerator::GridMeshData grid3;

	float					_asteroidSpeed[100];

	bool cam1state;
	bool cam2state;
	bool cam3state;
	bool cam4state;

	XMFLOAT3 camPos;
	XMFLOAT3 camObjPos;
	XMFLOAT3 objPos;


private:
	HRESULT InitWindow(HINSTANCE hInstance, int nCmdShow);
	HRESULT InitDevice();
	void Cleanup();
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
	HRESULT InitShadersAndInputLayout();
	HRESULT InitVertexBuffer();
	HRESULT InitGridVertexBuffer();
	HRESULT	InitGridIndexBuffer();
	HRESULT InitIndexBuffer();

	UINT _WindowHeight;
	UINT _WindowWidth;

public:
	Application();
	~Application();

	HRESULT Initialise(HINSTANCE hInstance, int nCmdShow);

	void Update();
	void Draw();
};

