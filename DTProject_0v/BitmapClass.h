#ifndef _BITMAPCLASS_H_
#define _BITMAPCLASS_H_

#include <D3D11.h>
#include <D3DX10math.h>

#include "TextureClass.h"

class BitmapClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
	};

public:
	BitmapClass();
	BitmapClass(const BitmapClass&);
	~BitmapClass();

	bool Initialize(ID3D11Device*, int, int, WCHAR*, int, int);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, int);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	bool UpdateBuffers(ID3D11DeviceContext*, int, int);
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer *_vertexBuffer, *_indexBuffer;
	int _vertexCount, _indexCount;
	TextureClass* _texture;

	int _screenWidth, _screenHeight;
	int _bitmapWidth, _bitmapHeight;
	int _previousPosX, _previousPosY;
};

#endif