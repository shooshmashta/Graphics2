#pragma once
#include "Defines.h"
struct ProjViewMatricies
{
	XMMATRIX world, view, proj;
};



struct DirectionalLight
{
	DirectionalLight()
	{
		ZeroMemory(this, sizeof(DirectionalLight));
	}
	
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 dir;
};

struct PointLight
{
	PointLight()
	{
		ZeroMemory(this, sizeof(PointLight));
	}
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
	XMFLOAT4 position;
	XMFLOAT4 direction;
	float range;
	XMFLOAT3 attentuation;
};

struct FullLight
{
	FullLight()
	{
		ZeroMemory(this, sizeof(FullLight));
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
	DirectionalLight  Directional;
	PointLight Point;
};


struct PointLights
{
	PointLights()
	{
		ZeroMemory(this, sizeof(PointLights));
	}
	void Set(XMFLOAT4 point, XMFLOAT4 color);

	XMFLOAT4 diffuse;
	XMFLOAT4 dir;
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