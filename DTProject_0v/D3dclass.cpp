#include "D3dclass.h"

D3DClass::D3DClass() :
	_swapChain(nullptr), _device(nullptr), _deviceContext(nullptr),
	_renderTargetView(nullptr), _depthStencilBuffer(nullptr),
	_depthStencilState(nullptr), _depthStencilView(nullptr),
	_rasterState(nullptr), _depthDisabledStencilState(nullptr)
{
}

D3DClass::D3DClass(const D3DClass& other)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync,
	HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;
	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;

	_vsync_enabled = vsync;

	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result)) {
		return false;
	}

	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result)) {
		return false;
	}

	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result)) {
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result)) {
		return false;
	}

	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList) {
		return false;
	}

	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result)) {
		return false;
	}

	for (int i = 0; i < numModes; i++) {
		if (displayModeList[i].Width == (unsigned int)screenWidth) {
			if (displayModeList[i].Height == (unsigned int)screenHeight) {
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result)) {
		return false;
	}

	_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	error = wcstombs_s(&stringLength, _videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0) {
		return false;
	}

	delete[] displayModeList;
	adapterOutput->Release();
	adapter->Release();
	factory->Release();

	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (_vsync_enabled) {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	if (fullscreen) {
		swapChainDesc.Windowed = false;
	}
	else {
		swapChainDesc.Windowed = true;
	}

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	featureLevel = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &_swapChain, &_device, NULL, &_deviceContext);
	if (FAILED(result)) {
		return false;
	}

	result = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result)) {
		return false;
	}

	result = _device->CreateRenderTargetView(backBufferPtr, NULL, &_renderTargetView);
	if (FAILED(result)) {
		return false;
	}

	backBufferPtr->Release();
	
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = _device->CreateTexture2D(&depthBufferDesc, NULL, &_depthStencilBuffer);
	if (FAILED(result)) {
		return false;
	}

	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = _device->CreateDepthStencilState(&depthStencilDesc, &_depthStencilState);
	if (FAILED(result)) {
		return false;
	}

	_deviceContext->OMSetDepthStencilState(_depthStencilState, 1);

	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = _device->CreateDepthStencilView(_depthStencilBuffer, &depthStencilViewDesc, &_depthStencilView);
	if (FAILED(result)) {
		return false;
	}
	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	result = _device->CreateRasterizerState(&rasterDesc, &_rasterState);
	if (FAILED(result)) {
		return false;
	}

	_deviceContext->RSSetState(_rasterState);

	viewport.Width = (float) screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	_deviceContext->RSSetViewports(1, &viewport);

	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;
	D3DXMatrixPerspectiveFovLH(&_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);
	D3DXMatrixIdentity(&_worldMatrix);
	D3DXMatrixOrthoLH(&_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = _device->CreateDepthStencilState(&depthDisabledStencilDesc, &_depthDisabledStencilState);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void D3DClass::Shutdown()
{
	if (_swapChain) {
		_swapChain->SetFullscreenState(false, NULL);
	}

	if (_depthDisabledStencilState) {
		_depthDisabledStencilState->Release();
	}

	if (_rasterState) {
		_rasterState->Release();
	}

	if (_depthStencilView) {
		_depthStencilView->Release();
	}

	if (_depthStencilState) {
		_depthStencilState->Release();
	}

	if (_depthStencilBuffer) {
		_depthStencilState->Release();
	}

	if (_renderTargetView) {
		_renderTargetView->Release();
	}

	if (_deviceContext) {
		_deviceContext->Release();
	}

	if (_device) {
		_device->Release();
	}

	if (_swapChain) {
		_swapChain->Release();
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	_deviceContext->ClearRenderTargetView(_renderTargetView, color);
	_deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	if (_vsync_enabled) {
		_swapChain->Present(1, 0);
	}
	else {
		_swapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return _device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return _deviceContext;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = _projectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = _worldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = _orthoMatrix;
	return;
}

void D3DClass::getVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, _videoCardDescription);
	memory = _videoCardMemory;
	return;
}

void D3DClass::TurnZBufferOn()
{
	_deviceContext->OMSetDepthStencilState(_depthStencilState, 1);
}

void D3DClass::TurnZBufferOff()
{
	_deviceContext->OMSetDepthStencilState(_depthDisabledStencilState, 1);
	return;
}