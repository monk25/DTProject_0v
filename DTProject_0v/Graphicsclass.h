#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

#include "D3dclass.h"
#include "CameraClass.h"
#include "ModelClass.h"

#include "ColorShaderClass.h"
#include "TextureShaderClass.h"
#include "LightShaderClass.h"
#include "LightClass.h"

#include "BitmapClass.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLE = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, D3DXVECTOR3);
	void CameraMove(D3DXVECTOR3 position);

private:
	bool Render(D3DXVECTOR3);

private:
	D3DClass * _d3d;
	CameraClass* _camera;
	ModelClass* _model;
	ColorShaderClass* _colorShader;
	TextureShaderClass* _textureShader;
	LightShaderClass* _lightShader;
	LightClass* _light;

	BitmapClass* _bitmap;
};

#endif