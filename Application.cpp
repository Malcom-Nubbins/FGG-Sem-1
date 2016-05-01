#include "Application.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

		case WM_LBUTTONDOWN:
			

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
	_pVertexBuffer = nullptr;
	_pIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;

	for (int i = 0; i < 100; i++)
	{
		_asteroidSpeed[i] = ((double)rand());
	}

	// Initialise Light values
	lightDirection = XMFLOAT3(0.25f, 0.5f, -1.0f);
	diffuseMaterial = XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f);
	diffuseLight = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	ambientMaterial = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	ambientLight = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	specularMaterial = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	specularLight = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	specularPower = 10.0f;

	//Initialise fog values
	gFogStart = 10.0f;
	gFogRange = 1000.0f;
	gFogColor = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
}

Application::~Application()
{
	Cleanup();
}

HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }
	

	// Initialize the world matrix
	XMStoreFloat4x4(&_skyBox, XMMatrixIdentity());
	XMStoreFloat4x4(&_world4, XMMatrixIdentity());
	XMStoreFloat4x4(&_world5, XMMatrixIdentity());
	for (int i = 0; i < 100; i++)
	{
		XMStoreFloat4x4(&_world6[i], XMMatrixIdentity());
	}

	XMStoreFloat4x4(&_gridWorld, XMMatrixIdentity());
	XMStoreFloat4x4(&_gridWorld3, XMMatrixIdentity());


	// Load objects
	objMeshData = OBJLoader::Load("objects/sphere.obj", _pd3dDevice);
	objMeshData2 = OBJLoader::Load("objects/donut.obj", _pd3dDevice);
	objMeshData3 = OBJLoader::Load("objects/Hercules.obj", _pd3dDevice);
	buildingMesh = OBJLoader::Load("objects/building.obj", _pd3dDevice);

	//Initialise Game Objects
	objMesh = new GameObject();
	objMesh->Init(objMeshData);

	objMesh2 = new GameObject();
	objMesh2->Init(objMeshData);

	objMesh3 = new GameObject();
	objMesh3->Init(objMeshData);

	objMesh4 = new GameObject();
	objMesh4->Init(objMeshData3);

	for (int i = 0; i < 100; i++)
	{
		objMesh6[i] = new GameObject();
		objMesh6[i]->Init(objMeshData2);
	}

	for (int i = 0; i < 20; i++)
	{
		buildingObject[i] = new GameObject();
		buildingObject[i]->Init(buildingMesh);
		buildingObject[i]->SetTranslation((rand() % 800 - 400), 0.0f, (rand() % 800 - 300));
		buildingObject[i]->SetScale(1.0f, 2.0f, 1.0f);
		buildingObject[i]->UpdateWorld();
	}

	// Initialise Cameras
	_camera.SetLens(XM_PIDIV2, _WindowWidth / _WindowHeight, 0.01f, 11000.0f);
	_camera.LookAt({ 1.0f, 0.0f, 0.0f }, { 0.0f, -1.0f, 0.0f}, { 0.0f, 1.0f, 0.0f });
	_camera.SetPosition(0.0f, 900.0f, 0.0f);

	_camera2.SetLens(XM_PIDIV2, _WindowWidth / _WindowHeight, 0.01, 11000.0f);
	_camera2.LookAt({1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f });
	_camera2.SetPosition(300.0f, 100.0f, -1000.0f);

	_camera3.SetLens(XM_PIDIV2, _WindowWidth / _WindowHeight, 0.01, 11000.0f);
	_camera3.LookAt({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f });
	_camera3.SetPosition(0.0f, 30.0f, 0.0f);

	objMesh4->StartDir({ -1.0f, 0.0f, 0.0f }, {0.0f, 0.0f, -1.0f}, { 0.0f, 0.0f, 1.0f });

	XMFLOAT3 cameraPos = _camera3.GetPosition();

	objMesh4->SetTranslation(cameraPos.x, 3.0f, cameraPos.z - 20);
	objMesh4->SetRotation(0.0f, 0.0f, 0.0f);
	camObjPos = objMesh4->GetPosition();
	objPos = objMesh4->GetPosition();

	_camera4.SetLens(XM_PIDIV2, _WindowWidth / _WindowHeight, 0.01, 11000.0f);
	_camera4.LookAt({ -1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f });


	return S_OK;
}



HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled VS.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled PS.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

	return hr;
}

HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    // Create vertex buffer
    SimpleVertex vertices[] =
    {
		{ XMFLOAT3( 1.0f, 1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) }, //Front Face
		{ XMFLOAT3(-1.0f, 1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, 
		{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) }, 
		{ XMFLOAT3( 1.0f,-1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) }, 

		{ XMFLOAT3( 1.0f, 1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 0.0f) }, //Right Face
		{ XMFLOAT3( 1.0f,-1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,-1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3( 1.0f, 1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3( 1.0f, 1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 0.0f) }, //Done
		{ XMFLOAT3( 1.0f, 1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, 1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,-1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f,-1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,-1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3( 1.0f,-1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3( 1.0f,-1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f,-1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3( 1.0f, 1.0f, 1.0f), XMFLOAT3( 0.0f, 0.0f,-1.0f), XMFLOAT2(1.0f, 1.0f) },

    };


    D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(SimpleVertex) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = vertices;

    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pVertexBuffer);

    if (FAILED(hr))
        return hr;

	HRESULT hr2;

	SimpleVertex triVertices[] =
	{
		{ XMFLOAT3(-1.0f, 0.0f, -1.0f), XMFLOAT3(-1.0f, 0.0f,-1.0f) },
		{ XMFLOAT3( 1.0f, 0.0f, -1.0f), XMFLOAT3( 1.0f, 0.0f,-1.0f) },
		{ XMFLOAT3( 1.0f, 0.0f,  1.0f), XMFLOAT3( 1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 0.0f,  1.0f), XMFLOAT3(-1.0f, 0.0f, 1.0f) },
		{ XMFLOAT3( 0.0f, 1.0f,  0.0f), XMFLOAT3( 0.0f, 1.0f, 0.0f) }
	};

	D3D11_BUFFER_DESC bd2;
	ZeroMemory(&bd2, sizeof(bd2));
	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.ByteWidth = sizeof(SimpleVertex) * 5;
	bd2.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd2.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitTriData;
	ZeroMemory(&InitTriData, sizeof(InitTriData));
	InitTriData.pSysMem = triVertices;

	hr2 = _pd3dDevice->CreateBuffer(&bd2, &InitTriData, &_pTriVertexBuffer);

	if (FAILED(hr2))
		return hr2;

	return S_OK;
}

HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    // Create index buffer
    WORD indices[] =
    {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4,
		8, 9, 10,
		10, 11, 8,
		12, 13, 14,
		14, 15, 12,
		16, 17, 18,
		18, 19, 16,
		20, 21, 22,
		22, 23, 20  //Tri12
    };


	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(WORD) * 36;     
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
    InitData.pSysMem = indices;
    hr = _pd3dDevice->CreateBuffer(&bd, &InitData, &_pIndexBuffer);

    if (FAILED(hr))
        return hr;

	HRESULT hr2;

	WORD triIndices[] = 
	{
		0, 1, 2, /* front left */
		0, 2, 3, /* front right*/
		4, 1, 0,
		4, 2, 1,
		4, 3, 2,
		4, 0, 3

	};

	D3D11_BUFFER_DESC bd2;
	ZeroMemory(&bd2, sizeof(bd2));

	bd2.Usage = D3D11_USAGE_DEFAULT;
	bd2.ByteWidth = sizeof(WORD) * 18;
	bd2.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd2.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitTriData;
	ZeroMemory(&InitTriData, sizeof(InitTriData));
	InitTriData.pSysMem = triIndices;
	hr2 = _pd3dDevice->CreateBuffer(&bd2, &InitTriData, &_pTriIndexBuffer);

	if (FAILED(hr2))
		return hr2;


	return S_OK;
}

HRESULT Application::InitGridVertexBuffer()
{
	HRESULT hr;

	GridGenerator gridGen;
	gridGen.CreateGrid(2000.0f, 2000.0f, 512, 512, grid, true); // Gives the terrain a size of 2000x2000 and a vertex size of 512x512, then sets it to use a heightmap

	mGridVertexCount = grid.Vertices.size();

	std::vector<SimpleVertex> gridVertices(mGridVertexCount);
	UINT k = 0;

	for (size_t i = 0; i < grid.Vertices.size(); i++, k++)
	{
		gridVertices[k].Pos = grid.Vertices[i].Position;
		gridVertices[k].Normal = grid.Vertices[i].Normal;
		gridVertices[k].TexC = grid.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(SimpleVertex) * mGridVertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGridData;
	ZeroMemory(&InitGridData, sizeof(InitGridData));
	InitGridData.pSysMem = &gridVertices[0];

	hr = _pd3dDevice->CreateBuffer(&vbd, &InitGridData, &_pGridVertexBuffer);

	if (FAILED(hr))
		return hr;

	HRESULT hr3;

	GridGenerator gridGen3;
	gridGen3.CreateGrid(7000.0f, 7000.0f, 100, 100, grid3, false);

	mGrid3VertexCount = grid3.Vertices.size();

	std::vector<SimpleVertex> grid3Vertices(mGrid3VertexCount);
	UINT k3 = 0;

	for (size_t i3 = 0; i3 < grid3.Vertices.size(); i3++, k3++)
	{
		grid3Vertices[k3].Pos = grid3.Vertices[i3].Position;
		grid3Vertices[k3].Normal = grid3.Vertices[i3].Normal;
		grid3Vertices[k3].TexC = grid3.Vertices[i3].TexC;
	}

	D3D11_BUFFER_DESC vbd3;
	ZeroMemory(&vbd3, sizeof(vbd3));
	vbd3.Usage = D3D11_USAGE_IMMUTABLE;
	vbd3.ByteWidth = sizeof(SimpleVertex) * mGrid3VertexCount;
	vbd3.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd3.CPUAccessFlags = 0;
	vbd3.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGrid3Data;
	ZeroMemory(&InitGrid3Data, sizeof(InitGrid3Data));
	InitGrid3Data.pSysMem = &grid3Vertices[0];

	hr3 = _pd3dDevice->CreateBuffer(&vbd3, &InitGrid3Data, &_pGrid3VertexBuffer);

	if (FAILED(hr3))
		return hr3;

	return S_OK;
}

HRESULT Application::InitGridIndexBuffer()
{
	HRESULT hri;
	mGridIndexCount = grid.Indices.size();

	std::vector<LONG> gridIndices;
	gridIndices.insert(gridIndices.end(), grid.Indices.begin(), grid.Indices.end());

	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(LONG) * mGridIndexCount;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGridIndexData;
	InitGridIndexData.pSysMem = &gridIndices[0];

	hri = _pd3dDevice->CreateBuffer(&ibd, &InitGridIndexData, &_pGridIndexBuffer);

	if (FAILED(hri))
		return hri;


	HRESULT hri3;
	mGrid3IndexCount = grid3.Indices.size();

	std::vector<LONG> grid3Indices;
	grid3Indices.insert(grid3Indices.end(), grid3.Indices.begin(), grid3.Indices.end());

	D3D11_BUFFER_DESC ibd3;
	ZeroMemory(&ibd3, sizeof(ibd3));
	ibd3.Usage = D3D11_USAGE_IMMUTABLE;
	ibd3.ByteWidth = sizeof(LONG) * mGrid3IndexCount;
	ibd3.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd3.CPUAccessFlags = 0;
	ibd3.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA InitGrid3IndexData;
	InitGrid3IndexData.pSysMem = &grid3Indices[0];

	hri3 = _pd3dDevice->CreateBuffer(&ibd3, &InitGrid3IndexData, &_pGrid3IndexBuffer);

	if (FAILED(hri3))
		return hri3;

	return S_OK;
}

HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 1280, 720};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"DX11 Framework", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

HRESULT Application::InitDevice()
{
    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = _WindowWidth;
	depthStencilDesc.Height = _WindowHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
	_pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);

    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

	InitShadersAndInputLayout();

	//Set the vertex buffers
	InitVertexBuffer();
	InitGridVertexBuffer();
 
	// Set the index buffers
	InitIndexBuffer();
	InitGridIndexBuffer();
   

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    if (FAILED(hr))
        return hr;

	// Creates the wireframe Rasterizer Desc
	D3D11_RASTERIZER_DESC wfdesc;
	ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc.FillMode = D3D11_FILL_WIREFRAME;
	wfdesc.CullMode = D3D11_CULL_NONE;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

	//Creates the solid fill Rasterizer desc
	D3D11_RASTERIZER_DESC wfdesc2;
	ZeroMemory(&wfdesc2, sizeof(D3D11_RASTERIZER_DESC));
	wfdesc2.FillMode = D3D11_FILL_SOLID;
	wfdesc2.CullMode = D3D11_CULL_BACK;
	hr = _pd3dDevice->CreateRasterizerState(&wfdesc2, &_solidFill);
	
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

	_pd3dDevice->CreateSamplerState(&sampDesc, &_pSamplerLinear);

	//Loads the textures for objects
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/Crate_COLOR.dds", nullptr, &_pTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/Pine Tree.dds", nullptr, &_pTreeTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/grass.dds", nullptr, &_grassTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/Hercules_COLOR.dds", nullptr, &_planeTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/ocean.dds", nullptr, &_oceanTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/sky.dds", nullptr, &_skyTextureRV);
	CreateDDSTextureFromFile(_pd3dDevice, L"textures/brick.dds", nullptr, &_buildingTextureRV);

	D3D11_BLEND_DESC blendDesc;
	ZeroMemory(&blendDesc, sizeof(blendDesc));

	D3D11_RENDER_TARGET_BLEND_DESC rtbd;
	ZeroMemory(&rtbd, sizeof(rtbd));

	rtbd.BlendEnable	= true;
	rtbd.SrcBlend		= D3D11_BLEND_SRC_COLOR;
	rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
	rtbd.BlendOp = D3D11_BLEND_OP_ADD;
	rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
	rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
	rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.RenderTarget[0] = rtbd;

	_pd3dDevice->CreateBlendState(&blendDesc, &Transparency);

    return S_OK;
}


void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pVertexBuffer) _pVertexBuffer->Release();
    if (_pIndexBuffer) _pIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();
	if (_depthStencilView) _depthStencilView->Release();
	if (_depthStencilBuffer) _depthStencilBuffer->Release();
	if (_wireFrame) _wireFrame->Release();
	if (_solidFill) _solidFill->Release();
	if (Transparency) Transparency->Release();
	if (_pTriVertexBuffer) _pTriVertexBuffer->Release();
	if (_pTriIndexBuffer) _pTriIndexBuffer->Release();
	if (_pGridVertexBuffer) _pGridVertexBuffer->Release();
	if (_pGrid3VertexBuffer) _pGrid3VertexBuffer->Release();
	if (_pGridIndexBuffer) _pGridIndexBuffer->Release();
	if (_pGrid3IndexBuffer) _pGrid3IndexBuffer->Release();
}

void Application::Update()
{
    // Update our time
    static float t = 0.0f;


    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / -5000.0f;
		
    }

	time = t;

	if (GetAsyncKeyState(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	short UKeyState = GetAsyncKeyState('U');

	if ((1 << 16) & UKeyState)
	{
		_pImmediateContext->RSSetState(_wireFrame);
	}

	short YKeyState = GetAsyncKeyState('Y');

	if ((1 << 16) & YKeyState)
	{
		_pImmediateContext->RSSetState(_solidFill);
	}

	// Definitions for input keys
	short LeftKeyState = GetAsyncKeyState('A');
	short ForwardKeyState = GetAsyncKeyState('W');
	short BackwardKeyState = GetAsyncKeyState('S');
	short RightKeyState = GetAsyncKeyState('D');
	short RotateLeftKeyState = GetAsyncKeyState(VK_LEFT);
	short RotateRightState = GetAsyncKeyState(VK_RIGHT);
	short PitchUpKeyState = GetAsyncKeyState(VK_DOWN);
	short PitchDownKeyState = GetAsyncKeyState(VK_UP);
	short Cam1KeyState = GetAsyncKeyState(VK_NUMPAD1);
	short Cam2KeyState = GetAsyncKeyState(VK_NUMPAD2);
	short Cam3KeyState = GetAsyncKeyState(VK_NUMPAD3);
	short Cam4KeyState = GetAsyncKeyState(VK_NUMPAD4);

	// Camera state switchers
	if ((0x8000) & Cam1KeyState)
	{
		cam1state = true;
		cam2state = false;
		cam3state = false;
		cam4state = false;
	}
	
	if ((0x8000) & Cam2KeyState)
	{
		cam1state = false;
		cam2state = true;
		cam3state = false;
		cam4state = false;
	}

	if ((0x8000) & Cam3KeyState)
	{
		cam1state = false;
		cam2state = false;
		cam3state = true;
		cam4state = false;
	}

	if ((0x8000) & Cam4KeyState)
	{
		cam1state = false;
		cam2state = false;
		cam3state = false;
		cam4state = true;
	}

	if (cam1state) // Camera 1: Birds-Eye View
	{
		_view = _camera.ViewFL();
		eyePosition = _camera.GetPosition();
		_projection = _camera.ProjFL();

		if ((0x8000) & LeftKeyState)
		{
			_camera.Strafe(-0.5f);
		}

		if ((0x8000) & RightKeyState)
		{
			_camera.Strafe(0.5f);
		}

		if ((0x8000) & ForwardKeyState)
		{
			_camera.Walk(0.5f);
		}

		if ((0x8000) & BackwardKeyState)
		{
			_camera.Walk(-0.5f);
		}

		if ((0x8000) & RotateLeftKeyState)
		{
			_camera.RotateY(-0.004f);
		}

		if ((0x8000) & RotateRightState)
		{
			_camera.RotateY(0.004f);
		}

		if ((0x8000) & PitchUpKeyState)
		{
			_camera.Pitch(-0.004f);
		}

		if ((0x8000) & PitchDownKeyState)
		{
			_camera.Pitch(0.004f);
		}

		_camera.UpdateViewMatrix();
		camPos = _camera.GetPosition();

		XMFLOAT3 camPos = _camera.GetPosition();

		XMStoreFloat4x4(&_skyBox, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f) * XMMatrixTranslation(camPos.x, camPos.y, camPos.z));
		XMStoreFloat4x4(&_gridWorld3, XMMatrixTranslation(camPos.x, -70.0f, camPos.z));
	}
	else if (cam2state) // Camera 2: Static Ground view
	{
		_view = _camera2.ViewFL();
		eyePosition = _camera2.GetPosition();
		_projection = _camera2.ProjFL();

		_camera2.UpdateViewMatrix();
		camPos = _camera2.GetPosition();

		XMFLOAT3 cam2Pos = _camera2.GetPosition();

		XMStoreFloat4x4(&_skyBox, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f) * XMMatrixTranslation(cam2Pos.x, cam2Pos.y, cam2Pos.z));
		XMStoreFloat4x4(&_gridWorld3, XMMatrixTranslation(cam2Pos.x, -70.0f, cam2Pos.z));
	}
	else if (cam3state) // Camera 3: Fuselage View
	{
		_view = _camera3.ViewFL();
		eyePosition = _camera3.GetPosition();
		_projection = _camera3.ProjFL();


		objMesh4->ForwardBackward(0.2f);
		camObjPos = objPos;

		
		_camera3.SetPosition(camObjPos.x, camObjPos.y + 5, camObjPos.z);

		XMFLOAT3 CamLook = _camera3.GetPosition();
		float rotationY = objMesh4->getRotationY();
		float rotationX = objMesh4->getRotationX();
		
		
		objMesh4->UpdateWorldPlane();

		if ((0x8000) & RotateLeftKeyState) // Rotates the plane left
		{
			//_camera3.Strafe(2.002f);
			_camera3.RotateY(-0.002f);
			objMesh4->LeftRight(-0.002f);
			objMesh4->SetRotation(rotationX, rotationY - 0.002f, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();

			_camera3.SetPosition(camObjPos.x, camObjPos.y + 5, camObjPos.z);
			_camera3.UpdateViewMatrix();
		}

		if ((0x8000) & RotateRightState) // Rotates the plane right
		{
			_camera3.RotateY(0.002f);
			objMesh4->LeftRight(0.002f);
			objMesh4->SetRotation(rotationX, rotationY  + 0.002f, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();
		}

		if ((0x8000) & PitchUpKeyState) // Pitches the plane upwards
		{
			_camera3.Pitch(-0.002f);
			objMesh4->UpDown(-0.002f);
			objMesh4->SetRotation(rotationX + 0.002f, rotationY, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();
		}

		if ((0x8000) & PitchDownKeyState) // Pitches the plane downwards
		{
			_camera3.Pitch(0.002f);
			objMesh4->UpDown(0.002f);
			objMesh4->SetRotation(rotationX - 0.002f, rotationY, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();
		}

		
		_camera3.UpdateViewMatrix();
		camPos = _camera3.GetPosition();

		XMFLOAT3 cam3Pos = _camera3.GetPosition();

		XMStoreFloat4x4(&_skyBox, XMMatrixScaling(5000.0f, 5000.0f, 5000.0f) * XMMatrixTranslation(cam3Pos.x, cam3Pos.y, cam3Pos.z));
		XMStoreFloat4x4(&_gridWorld3, XMMatrixTranslation(cam3Pos.x, -70.0f, cam3Pos.z));
	}
	else if (cam4state) //3rd Person Aircraft view
	{
		_view = _camera4.ViewFL();
		eyePosition = _camera4.GetPosition();
		_projection = _camera4.ProjFL();

		objMesh4->ForwardBackward(0.2f);
		camObjPos = objPos;


		_camera4.SetPosition(camObjPos.x, camObjPos.y + 15, camObjPos.z + 30);
		objMesh4->UpdateWorldPlane();

		float rotationY = objMesh4->getRotationY();
		float rotationX = objMesh4->getRotationX();

		if ((0x8000) & RotateLeftKeyState) // Rotates the plane left
		{
			//_camera3.Strafe(2.002f);
			_camera3.RotateY(-0.002f);
			objMesh4->LeftRight(-0.002f);
			objMesh4->SetRotation(rotationX, rotationY - 0.002f, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();

			_camera4.SetPosition(camObjPos.x, camObjPos.y + 15, camObjPos.z + 30);
			_camera4.UpdateViewMatrix();
		}

		if ((0x8000) & RotateRightState) // Rotates the plane right
		{
			_camera3.RotateY(0.002f);
			objMesh4->LeftRight(0.002f);
			objMesh4->SetRotation(rotationX, rotationY + 0.002f, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();
		}

		if ((0x8000) & PitchUpKeyState) // Pitches the plane upwards
		{
			_camera3.Pitch(-0.002f);
			objMesh4->UpDown(-0.002f);
			objMesh4->SetRotation(rotationX + 0.002f, rotationY, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();
		}

		if ((0x8000) & PitchDownKeyState) // Pitches the plane downwards
		{
			_camera3.Pitch(0.002f);
			objMesh4->UpDown(0.002f);
			objMesh4->SetRotation(rotationX - 0.002f, rotationY, 0.0f);
			camObjPos = objPos;
			objMesh4->UpdateWorldPlane();
			objMesh4->UpdateWorld();
		}

		
		XMFLOAT3 cam4Pos = _camera4.GetPosition();
		_camera3.UpdateViewMatrix();
		_camera4.UpdateViewMatrix();
		XMStoreFloat4x4(&_skyBox, XMMatrixScaling(6800.0f, 6800.0f, 6800.0f) * XMMatrixTranslation(cam4Pos.x, cam4Pos.y, cam4Pos.z));
		XMStoreFloat4x4(&_gridWorld3, XMMatrixTranslation(cam4Pos.x, -70.0f, cam4Pos.z));
		camPos = _camera4.GetPosition();
	}

	
	objPos = objMesh4->GetPosition();

    //
    // Animate the Game objects
    //
	objMesh->SetScale(3.0f, 3.0f, 3.0f); objMesh->SetRotation(5 * t, 5 * t, 5 * t); objMesh->SetTranslation(0.0f, 100.0f, 0.0f);
	objMesh->UpdateWorld();

	//Planet 1
	objMesh2->SetScale(1.0f, 1.0f, 1.0f); objMesh2->SetTranslation(8.0, 100.0f, 0); objMesh2->SetRotation(7 * t, 0, 0); objMesh2->SetYaw(7 * t);
	objMesh2->UpdateWorld();

	//Planet 2
	objMesh3->SetScale(1.5f, 1.5f, 1.5f); objMesh3->SetTranslation(-19.0f, 100.0f, 0.0f); objMesh3->SetRotation(-5 * t, 0, 0); objMesh3->SetYaw(-3 * t);
	objMesh3->UpdateWorld();

	objMesh4->UpdateWorld();

	XMStoreFloat4x4(&_world4, XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(7 * t) * XMMatrixTranslation(3.0f, 0.0f, 0.0f) * XMMatrixRotationY(7 * t) * XMMatrixTranslation(8.0f, 100.0f, 0.0f) * XMMatrixRotationY(7 * t)); //Planet 1 Moon
	XMStoreFloat4x4(&_world5, XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixRotationY(10 * t) * XMMatrixTranslation(6.0f, 0.0f, 0.0f) * XMMatrixRotationY(9 * t) * XMMatrixTranslation(-19.0f, 100.0f, 0.0f) * XMMatrixRotationY(-3 * t)); //Planet 2 Moon

	for (int i = 0; i < 100; i++)
	{
		//XMStoreFloat4x4(&objMesh6[i]->GetWorld(), XMMatrixScaling(0.1f, 0.1f, 0.1f) * XMMatrixRotationY(6 * t) * XMMatrixTranslation(9.0f, 0.0f, 0.0f) * XMMatrixRotationY(7*t) * XMMatrixRotationY(_asteroidSpeed[i]));
		objMesh6[i]->SetScale(0.6, 0.6, 0.6); objMesh6[i]->SetRotation(t, 6 * t, 0); objMesh6[i]->SetTranslation(25.0f, 100.0f, 0);  objMesh6[i]->SetYaw(_asteroidSpeed[i]);
		objMesh6[i]->UpdateWorld();
	}

	XMStoreFloat4x4(&_gridWorld , XMMatrixTranslation(0.0f, -80.0f, 0.0f));

}

void Application::Draw()
{

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;

    //
    // Clear the back buffer
    //
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);

	_pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	float blendFactor[] = { 0.35f, 0.35f, 0.35f, 1.0f };

	_pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);

	XMMATRIX world = XMLoadFloat4x4(&objMesh->GetWorld());
	XMMATRIX view = XMLoadFloat4x4(&_view);
	XMMATRIX projection = XMLoadFloat4x4(&_projection);

    //
    // Update variables
    //
    ConstantBuffer cb;
	cb.mWorld = XMMatrixTranspose(world);
	cb.mView = XMMatrixTranspose(view);
	cb.mProjection = XMMatrixTranspose(projection);
	cb.LightVecW = lightDirection;
	cb.DiffuseMtrl = diffuseMaterial;
	cb.DiffuseLight = diffuseLight;
	cb.AmbientMtrl = ambientMaterial;
	cb.AmbientLight = ambientLight;
	cb.SpecularMtrl = specularMaterial;
	cb.SpecularLight = specularLight;
	cb.SpecularPower = specularPower;
	cb.pad = XMFLOAT3(0.0f, 0.0f, 0.0f);
	cb.EyePosW = eyePosition;
	cb.pad3 = 0.0f;
	cb.time = time;
	cb.gFogStart = gFogStart;
	cb.gFogRange = gFogRange;
	cb.pad2 = XMFLOAT2(0.0f, 0.0f);
	cb.gFogColor = gFogColor;
	cb.gFogEnabled = 1.0f;

	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    //
    // Renders a triangle
    //
	_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
	_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
	_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	_pImmediateContext->PSSetSamplers(0, 1, &_pSamplerLinear);
	objMesh->Draw(_pd3dDevice, _pImmediateContext);

	XMMATRIX world2 = XMLoadFloat4x4(&objMesh2->GetWorld());
	cb.mWorld = XMMatrixTranspose(world2);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	objMesh2->Draw(_pd3dDevice, _pImmediateContext);

	XMMATRIX world3 = XMLoadFloat4x4(&objMesh3->GetWorld());
	cb.mWorld = XMMatrixTranspose(world3);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	objMesh3->Draw(_pd3dDevice, _pImmediateContext);

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pTriVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pTriIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	XMMATRIX world4 = XMLoadFloat4x4(&_world4);
	cb.mWorld = XMMatrixTranspose(world4);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(18, 0, 0);

	XMMATRIX world5 = XMLoadFloat4x4(&_world5);
	cb.mWorld = XMMatrixTranspose(world5);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(18, 0, 0);

	cb.gFogEnabled = 0.0f;
	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	_pImmediateContext->PSSetShaderResources(0, 1, &_skyTextureRV);

	XMMATRIX skyBox = XMLoadFloat4x4(&_skyBox);
	cb.mWorld = XMMatrixTranspose(skyBox);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(36, 0, 0);

	cb.gFogEnabled = 1.0f;
	_pImmediateContext->PSSetShaderResources(0, 1, &_pTextureRV);
	

	for (int i = 0; i < 100; i++)
	{
		XMMATRIX world6 = XMLoadFloat4x4(&objMesh6[i]->GetWorld());
		cb.mWorld = XMMatrixTranspose(world6);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		objMesh6[i]->Draw(_pd3dDevice, _pImmediateContext);
	}

	_pImmediateContext->PSSetShaderResources(0, 1, &_buildingTextureRV);
	for (int i = 0; i < 20; i++)
	{
		XMMATRIX building = XMLoadFloat4x4(&buildingObject[i]->GetWorld());
		cb.mWorld = XMMatrixTranspose(building);
		_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
		buildingObject[i]->Draw(_pd3dDevice, _pImmediateContext);
	}

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pGridVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pGridIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	
	_pImmediateContext->PSSetShaderResources(0, 1, &_grassTextureRV);

	XMMATRIX world7 = XMLoadFloat4x4(&_gridWorld);
	cb.mWorld = XMMatrixTranspose(world7);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(mGridIndexCount, 0, 0);

	_pImmediateContext->IASetVertexBuffers(0, 1, &_pVertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_pImmediateContext->PSSetShaderResources(0, 1, &_planeTextureRV);

	XMMATRIX world9 = XMLoadFloat4x4(&objMesh4->GetWorld());
	cb.mWorld = XMMatrixTranspose(world9);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	objMesh4->Draw(_pd3dDevice, _pImmediateContext);


	_pImmediateContext->IASetVertexBuffers(0, 1, &_pGrid3VertexBuffer, &stride, &offset);
	_pImmediateContext->IASetIndexBuffer(_pGrid3IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	_pImmediateContext->PSSetShaderResources(0, 1, &_oceanTextureRV);
	_pImmediateContext->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

	XMMATRIX world10 = XMLoadFloat4x4(&_gridWorld3);
	cb.mWorld = XMMatrixTranspose(world10);
	_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	_pImmediateContext->DrawIndexed(mGrid3IndexCount, 0, 0);


    //
    // Present our back buffer to our front buffer
    //
    _pSwapChain->Present(0, 0);
}