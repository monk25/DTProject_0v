#include "Inputclass.h"

InputClass::InputClass()
{
	_directInput = nullptr;
	_keyboard = nullptr;
	_mouse = nullptr;
}

InputClass::InputClass(const InputClass& other)
{
}

InputClass::~InputClass()
{
}

bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;

	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	_mouseX = 0;
	_mouseY = 0;

	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&_directInput, NULL);
	if (FAILED(result)) {
		return false;
	}

	result = _directInput->CreateDevice(GUID_SysKeyboard, &_keyboard, NULL);
	if (FAILED(result)) {
		return false;
	}

	result = _keyboard->SetDataFormat(&c_dfDIKeyboard);
	if (FAILED(result)) {
		return false;
	}

	result = _keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	result = _keyboard->Acquire();
	if (FAILED(result)) {
		return false;
	}

	result = _directInput->CreateDevice(GUID_SysMouse, &_mouse, NULL);
	if (FAILED(result)) {
		return false;
	}

	result = _mouse->SetDataFormat(&c_dfDIMouse);
	if (FAILED(result)) {
		return false;
	}

	result = _mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if (FAILED(result)) {
		return false;
	}

	result = _mouse->Acquire();
	if (FAILED(result)) {
		return false;
	}

	for (int i = 0; i < 256; i++) {
		_keys[i] = false;
	}

	return true;
}

void InputClass::Shutdown()
{
	if (_mouse) {
		_mouse->Unacquire();
		_mouse->Release();
	}

	if (_keyboard) {
		_keyboard->Unacquire();
		_keyboard->Release();
	}

	if (_directInput) {
		_directInput->Release();
	}

	return;
}

bool InputClass::Frame()
{
	bool result;

	result = ReadKeyboard();
	if (!result) {
		return false;
	}

	result = ReadMouse();
	if (!result) {
		return false;
	}

	ProcessInput();

	return true;
}

bool InputClass::ReadKeyboard()
{
	HRESULT result;

	result = _keyboard->GetDeviceState(sizeof(_keyboardState), (LPVOID)&_keyboardState);
	if (FAILED(result)) {
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			_keyboard->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

bool InputClass::ReadMouse()
{
	HRESULT result;

	result = _mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&_mouseState);
	if (FAILED(result)) {
		if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED)) {
			_mouse->Acquire();
		}
		else {
			return false;
		}
	}

	return true;
}

void InputClass::ProcessInput()
{
	_mouseX += _mouseState.lX;
	_mouseY += _mouseState.lY;

	if (_mouseX < 0) { _mouseX = 0; }
	if (_mouseY < 0) { _mouseY = 0; }

	if (_mouseX > _screenWidth) { _mouseX = _screenWidth; }
	if (_mouseY > _screenHeight) { _mouseY = _screenHeight; }

	return;
}

bool InputClass::IsKeyPrssed(int dik)
{
	return (_keyboardState[dik] & 0x80);
}

bool InputClass::IsEscapePressed()
{
	if (_keyboardState[DIK_ESCAPE] & 0x80) {
		return true;
	}

	return false;
}

void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = _mouseX;
	mouseY = _mouseY;
	return;
}

void InputClass::KeyDown(unsigned int input)
{
	_keys[input] = true;
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	_keys[input] = false;
	return;
}

bool InputClass::IsKeyDown(unsigned int key)
{
	return _keys[key];
}