#include "Graphicsclass.h"

GraphicsClass::GraphicsClass() : _d3d(nullptr), _camera(nullptr), _model(nullptr), 
_colorShader(nullptr), _textureShader(nullptr), _lightShader(nullptr), _light(nullptr), _bitmap(nullptr)
{
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	_d3d = new D3DClass;
	if (!_d3d) {
		return false;
	}

	result = _d3d->Initialize(screenWidth, screenHeight, VSYNC_ENABLE, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	_camera = new CameraClass;
	if (!_camera) {
		return false;
	}

	_model = new ModelClass;
	if (!_model) {
		return false;
	}
	_camera->SetPosition(0.0f, 0.0f, -5.0f);

	result = _model->Initialize(_d3d->GetDevice(), const_cast<WCHAR*>(L"../Resource/seafloor.dds"));
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
		return false;
	}

	_colorShader = new ColorShaderClass;
	if (!_colorShader) {
		return false;
	}

	result = _colorShader->Initialize(_d3d->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	_textureShader = new TextureShaderClass;
	if (!_textureShader) {
		return false;
	}

	result = _textureShader->Initialize(_d3d->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}

	_bitmap = new BitmapClass;
	if (!_bitmap) {
		return false;
	}

	result = _bitmap->Initialize(_d3d->GetDevice(), screenWidth, screenHeight, const_cast<WCHAR*>(L"../Resource/seafloor.dds"), 256, 256);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the bitmap object.", L"Error", MB_OK);
		return false;
	}

	_lightShader = new LightShaderClass;
	if (!_lightShader){
		return false;
	}

	result = _lightShader->Initialize(_d3d->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, L"Could not initialize the light shader object.", L"Error", MB_OK);
		return false;
	}

	_light = new LightClass;
	if (!_light) {
		return false;
	}

	_light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	_light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	_light->SetDirection(0.0f, 0.0f, 1.0f);
	_light->SetSpecularColor(1.0f, 1.0f, 1.0f, 1.0f);
	_light->SetSpecularPower(32.0f);

	return true;
}

void GraphicsClass::Shutdown()
{
	if (_light) {
		delete _light;
	}

	if (_lightShader) {
		_lightShader->Shutdown();
		delete _lightShader;
	}

	if (_colorShader) {
		_colorShader->Shutdown();
		delete _colorShader;
	}

	if (_bitmap) {
		_bitmap->Shutdown();
		delete _bitmap;
	}

	if (_textureShader) {
		_textureShader->Shutdown();
		delete _textureShader;
	}

	if (_model) {
		_model->Shutdown();
		delete _model;
	}

	if (_camera) {
		delete _camera;
	}

	if (_d3d) {
		_d3d->Shutdown();
		delete _d3d;
	}

	return;
}

bool GraphicsClass::Frame(int mouseX, int mouseY, D3DXVECTOR3 rotation)
{
	bool result;

	result = Render(rotation);
	if (!result) {
		return false;
	}

	return true;
}

bool GraphicsClass::Render(D3DXVECTOR3 rotation)
{
	D3DXMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoMatrix;
	bool result;

	_d3d->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	_camera->Render();

	_camera->GetViewMatrix(viewMatrix);
	_d3d->GetWorldMatrix(worldMatrix);
	_d3d->GetProjectionMatrix(projectionMatrix);

	//D3DXMatrixRotationX(&worldMatrix, rotation.x);
	//D3DXMatrixRotationY(&worldMatrix, rotation.y);
	//D3DXMatrixRotationZ(&worldMatrix, rotation.z);


	D3DXMatrixRotationYawPitchRoll(&worldMatrix, rotation.y, rotation.x, rotation.z);

	_model->Render(_d3d->GetDeviceContext());

	result = _lightShader->Render(_d3d->GetDeviceContext(), _model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, _model->GetTexture(), _light->GetDirection(), _light->GetAmbientColor(), _light->GetDiffuseColor(), _camera->GetPosition(), _light->GetSpecularColor(), _light->GetSpecularPower());
	if (!result) {
		return false;
	}


	_d3d->GetOrthoMatrix(orthoMatrix);
	_d3d->TurnZBufferOff();

	result = _bitmap->Render(_d3d->GetDeviceContext(), 100, 100);
	if (!result) {
		return false;
	}

	result = _textureShader->Render(_d3d->GetDeviceContext(), _bitmap->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, _bitmap->GetTexture());
	if (!result) {
		return false;
	}

	_d3d->TurnZBufferOn();

	_d3d->EndScene();

	return true;
}

void GraphicsClass::CameraMove(D3DXVECTOR3 position)
{
	D3DXVECTOR3 pos = _camera->GetPosition() + position;
	_camera->SetPosition(pos.x, pos.y, pos.z);
}