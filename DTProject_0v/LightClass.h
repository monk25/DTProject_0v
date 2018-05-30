#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

#include <D3DX10math.h>

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetSpecularPower(float);

	D3DXVECTOR4 GetAmbientColor();
	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR3 GetDirection();
	D3DXVECTOR4 GetSpecularColor();
	float GetSpecularPower();

private:
	D3DXVECTOR4 _ambientColor;
	D3DXVECTOR4 _diffuseColor;
	D3DXVECTOR3 _direction;
	D3DXVECTOR4 _specularColor;
	float _specularPower;
};

#endif