#include "Systemclass.h"

SystemClass::SystemClass() : 
	_Input(nullptr), _Graphics(nullptr), worldRotation(0.0f, 0.0f, 0.0f)
{
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	_Input = new InputClass;
	if (!_Input) {
		return false;
	}

	result = _Input->Initialize(_hinstance, _hwnd, screenWidth,  screenHeight);
	if (!result) {
		MessageBox(_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	_Graphics = new GraphicsClass;
	if (!_Graphics) {
		return false;
	}

	result = _Graphics->Initialize(screenWidth, screenHeight, _hwnd);
	if (!result) {
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	if (_Graphics) {
		_Graphics->Shutdown();
		delete _Graphics;
	}

	if (_Input) {
		_Input->Shutdown();
		delete _Input;
	}

	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done) {
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			done = true;
		}
		else {
			result = Frame();
			if (!result) {
				MessageBox(_hwnd, L"Frame Processing Failed", L"Error", MB_OK);
				done = true;
			}
			if (_Input->IsEscapePressed()) {
				done = true;
			}
		}
	}
}

bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;

	result = _Input->Frame();
	if (!result) {
		return false;
	}
	_Input->GetMouseLocation(mouseX, mouseY);

	//if (_Input->IsKeyDown(VK_ESCAPE)) {
	//	return false;
	//}

	D3DXVECTOR3 cameraMovePos(0.0f, 0.0f, 0.0f);
	if (_Input->IsKeyPrssed(DIK_LEFT)) {
		cameraMovePos.x -= 0.1f;
	}
	if (_Input->IsKeyPrssed(DIK_RIGHT)) {
		cameraMovePos.x += 0.1f;
	}
	if (_Input->IsKeyPrssed(DIK_UP)) {
		cameraMovePos.y += 0.1f;
	}
	if (_Input->IsKeyPrssed(DIK_DOWN)) {
		cameraMovePos.y -= 0.1f;
	}
	_Graphics->CameraMove(cameraMovePos);

	if (_Input->IsKeyPrssed(DIK_A)) {
		worldRotation.y += (float)D3DX_PI * 0.005f;
	}
	if (_Input->IsKeyPrssed(DIK_D)) {
		worldRotation.y -= (float)D3DX_PI * 0.005f;
	}
	if (_Input->IsKeyPrssed(DIK_W)) {
		worldRotation.x += (float)D3DX_PI * 0.005f;
	}
	if (_Input->IsKeyPrssed(DIK_S)) {
		worldRotation.x -= (float)D3DX_PI * 0.005f;
	}
	if (_Input->IsKeyPrssed(DIK_Q)) {
		worldRotation.z += (float)D3DX_PI * 0.005f;
	}
	if (_Input->IsKeyPrssed(DIK_E)) {
		worldRotation.z -= (float)D3DX_PI * 0.005f;
	}

	result = _Graphics->Frame(mouseX, mouseY, worldRotation);
	if (!result) {
		return false;
	}

	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	//switch (umsg) {
	//case WM_KEYDOWN:
	//	_Input->KeyDown((unsigned int)wparam);
	//	return 0;
	//	
	//case WM_KEYUP:
	//	_Input->KeyUp((unsigned int)wparam);
	//
	//default:
	//	return DefWindowProc(hwnd, umsg, wparam, lparam);
	//}
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;
	_hinstance = GetModuleHandle(NULL);
	_applicationName = L"Engine";

	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= _hinstance;
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm			= wc.hIcon;
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName		= NULL;
	wc.lpszClassName	= _applicationName;
	wc.cbSize			= sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth	 = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN) {
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize			= sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth	= screenWidth;
		dmScreenSettings.dmBitsPerPel	= 32;
		dmScreenSettings.dmFields		= DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else {
		screenWidth	 = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	_hwnd = CreateWindowEx(WS_EX_APPWINDOW, _applicationName, _applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY,
		screenWidth, screenHeight, NULL, NULL, _hinstance, NULL);

	ShowWindow(_hwnd, SW_SHOW);
	SetForegroundWindow(_hwnd);
	SetFocus(_hwnd);

	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN) {
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(_hwnd);
	_hwnd = NULL;

	UnregisterClass(_applicationName, _hinstance);
	_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;

	default :
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
}