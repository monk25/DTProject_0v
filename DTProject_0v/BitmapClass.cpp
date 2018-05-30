#include "BitmapClass.h"

BitmapClass::BitmapClass()
{
	_vertexBuffer = nullptr;
	_indexBuffer = nullptr;
	_texture = nullptr;
}

BitmapClass::BitmapClass(const BitmapClass& other)
{
}

BitmapClass::~BitmapClass()
{
}

bool BitmapClass::Initialize(ID3D11Device* device, int screenWidth, int screenHeight, WCHAR* textureFilename, int bitmapWidth, int bitmapHeight)
{
	bool result;
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;

	_bitmapWidth = bitmapWidth;
	_bitmapHeight = bitmapHeight;

	_previousPosX = -1;
	_previousPosY = -1;

	result = InitializeBuffers(device);
	if (!result) {
		return false;
	}

	result = LoadTexture(device, textureFilename);
	if (!result) {
		return false;
	}

	return true;
}

void BitmapClass::Shutdown()
{
	ReleaseTexture();
	ShutdownBuffers();

	return;
}

bool BitmapClass::Render(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	bool result;

	result = UpdateBuffers(deviceContext, positionX, positionY);
	if (!result) {
		return false;
	}

	RenderBuffers(deviceContext);

	return true;
}

int BitmapClass::GetIndexCount()
{
	return _indexCount;
}

ID3D11ShaderResourceView* BitmapClass::GetTexture()
{
	return _texture->GetTexture();
}

bool BitmapClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	_vertexCount = 6;
	_indexCount = _vertexCount;

	vertices = new VertexType[_vertexCount];
	if (!vertices) {
		return false;
	}

	indices = new unsigned long[_indexCount];
	if (!indices) {
		return false;
	}

	memset(vertices, 0, (sizeof(VertexType) * _vertexCount));

	for (int i = 0; i < _indexCount; i++) {
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * _vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &_vertexBuffer);
	if (FAILED(result)) {
		return false;
	}

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * _indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &_indexBuffer);
	if (FAILED(result)) {
		return false;
	}

	delete[] vertices;
	vertices = nullptr;

	delete[] indices;
	indices = nullptr;

	return true;
}

void BitmapClass::ShutdownBuffers()
{
	if (_indexBuffer) {
		_indexBuffer->Release();
		_indexBuffer = nullptr;
	}

	if (_vertexBuffer) {
		_vertexBuffer->Release();
		_vertexBuffer = nullptr;
	}

	return;
}

bool BitmapClass::UpdateBuffers(ID3D11DeviceContext* deviceContext, int positionX, int positionY)
{
	float left, right, top, bottom;
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	if ((positionX == _previousPosX) && positionY == _previousPosY){
		return true;
	}

	_previousPosX = positionX;
	_previousPosY = positionY;

	left = (float)((_screenWidth / 2) * -1) + (float)positionX;
	right = left + (float)_bitmapWidth;
	top = (float)(_screenHeight / 2) - (float)positionY;
	bottom = top - (float)_bitmapHeight;

	vertices = new VertexType[_vertexCount];
	if (!vertices) {
		return false;
	}

	vertices[0].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[0].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[1].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[1].texture = D3DXVECTOR2(1.0f, 1.0f);

	vertices[2].position = D3DXVECTOR3(left, bottom, 0.0f);
	vertices[2].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[3].position = D3DXVECTOR3(left, top, 0.0f);
	vertices[3].texture = D3DXVECTOR2(0.0f, 0.0f);

	vertices[4].position = D3DXVECTOR3(right, top, 0.0f);
	vertices[4].texture = D3DXVECTOR2(1.0f, 0.0f);

	vertices[5].position = D3DXVECTOR3(right, bottom, 0.0f);
	vertices[5].texture = D3DXVECTOR2(1.0f, 1.0f);

	result = deviceContext->Map(_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result)) {
		return false;
	}

	verticesPtr = (VertexType*)mappedResource.pData;

	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * _vertexCount));

	deviceContext->Unmap(_vertexBuffer, 0);

	delete[] vertices;
	vertices = nullptr;

	return true;
}

void BitmapClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool BitmapClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	_texture = new TextureClass;
	if (!_texture) {
		return false;
	}

	result = _texture->Initialize(device, filename);
	if (!result) {
		return false;
	}

	return true;
}

void BitmapClass::ReleaseTexture()
{
	if (_texture) {
		_texture->Shutdown();
		delete _texture;
		_texture = nullptr;
	}

	return;
}