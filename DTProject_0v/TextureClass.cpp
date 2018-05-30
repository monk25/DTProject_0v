#include "TextureClass.h"

TextureClass::TextureClass()
{
	_texture = nullptr;
}

TextureClass::TextureClass(const TextureClass& other)
{
}

TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result;

	result = D3DX11CreateShaderResourceViewFromFile(device, filename, NULL, NULL, &_texture, NULL);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	if (_texture) {
		_texture->Release();
		_texture = nullptr;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return _texture;
}