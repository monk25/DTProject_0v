#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

#define DIRECTINPUT_VERSION 0x0800

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#include <dinput.h>

class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsKeyPrssed(int dik);
	bool IsEscapePressed();
	void GetMouseLocation(int&, int&);

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

	IDirectInput8* _directInput;
	IDirectInputDevice8* _keyboard;
	IDirectInputDevice8* _mouse;

	bool _keys[256];
	unsigned char _keyboardState[256];
	DIMOUSESTATE _mouseState;

	int _screenWidth, _screenHeight;
	int _mouseX, _mouseY;
};

#endif