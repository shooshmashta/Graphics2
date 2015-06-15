#pragma once
#include"ObjectModel.h"
struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 position;
};

struct FullLight
{
	FullLight()
	{
		ZeroMemory(this, sizeof(Light));
	}

	XMFLOAT4 diffuse;
	XMFLOAT4 ambient;
	XMFLOAT4 specular;
	XMFLOAT4 emissive;
	XMFLOAT4 power;
	XMFLOAT4 position;


};

struct toPixelShader
{
	Light  Directional;
};
