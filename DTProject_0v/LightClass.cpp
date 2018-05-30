#include "LightClass.h"

LightClass::LightClass()
{
}

LightClass::LightClass(const LightClass& other)
{
}

LightClass::~LightClass()
{
}

void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	_ambientColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetDirection(float x, float y, float z)
{
	_direction = D3DXVECTOR3(x, y, z);
	return;
}

void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	_specularColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}

void LightClass::SetSpecularPower(float power)
{
	_specularPower = power;
	return;
}

D3DXVECTOR4 LightClass::GetAmbientColor()
{
	return _ambientColor;
}

D3DXVECTOR4 LightClass::GetDiffuseColor()
{
	return _diffuseColor;
}

D3DXVECTOR3 LightClass::GetDirection()
{
	return _direction;
}

D3DXVECTOR4 LightClass::GetSpecularColor()
{
	return _specularColor;
}

float LightClass::GetSpecularPower()
{
	return _specularPower;
}