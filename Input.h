#pragma once

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#include <dinput.h>
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

using namespace DirectX;

class Input
{
public:
	Input();
	~Input();
	bool InitDirectInput(HINSTANCE hInstance);
	void CleanUp();

	float rotX = 0;
	float rotZ = 0;
	float moveUD = 0;
	float moveLR = 0;

private:
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;

	XMMATRIX RotationX;
	XMMATRIX RotationZ;
};

