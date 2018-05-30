#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

#include <D3DX10math.h>

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	float _positionX, _positionY, _positionZ;
	float _rotationX, _rotationY, _rotationZ;
	D3DXMATRIX _viewMatrix;
};

#endif