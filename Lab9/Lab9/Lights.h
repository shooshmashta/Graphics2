#pragma once
#include "Defines.h"
struct ProjViewMatricies
{
	XMMATRIX view, proj;
};



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

struct lightStruct
{
	Light  Directional;
};

struct Lights
{
	
	lightStruct light;

	ID3D11Buffer * LightBuffer;

	bool Init(XMFLOAT3 pos,
		ID3D11Device * dev,
		ID3D11DeviceContext *devCon,
		ProjViewMatricies* _viewproj);

	bool Run(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	void Cleanup();
};