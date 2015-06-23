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
	
	XMFLOAT3 dir;
	float pad;
	
	XMFLOAT4 color;
	XMFLOAT4 ambient;
};

struct PointLight
{
	PointLight()
	{
		ZeroMemory(this, sizeof(PointLight));
	}
	XMFLOAT3 position;
	float  PointRange;
	
	XMFLOAT4 direction;
	
	XMFLOAT3 attentuation;
	float Padding;

	XMFLOAT4 color;
	XMFLOAT4 ambient;
};

struct SpotLight
{
	SpotLight()
	{
		ZeroMemory(this, sizeof(SpotLight));
	}

	XMFLOAT3 position;
	float  spotRange;

	XMFLOAT3 direction;
	float SpotCone;

	XMFLOAT3 attentuation;
	float paddddd;

	XMFLOAT4 color;
	XMFLOAT4 ambient;
};

struct lightStruct
{
	DirectionalLight  Directional;
	//PointLight Point;
	//SpotLight Spot;
	//SpecLight Spec;
};

struct AllofTheLights
{
	DirectionalLight  Directional;
	PointLight Point;
	SpotLight Spot;
	
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
	AllofTheLights fourlights;
	ID3D11Buffer * LightBuffer;
	SpotLight On, off;
	bool spot = true;
	/*ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;*/




	bool Init(XMFLOAT3 pos,
		ID3D11Device * dev,
		ID3D11DeviceContext *devCon,
		ProjViewMatricies* _viewproj);

	bool LightsInit(
		ID3D11Device * dev,
		ID3D11DeviceContext *devCon,
		ProjViewMatricies* _viewproj);

	bool Run(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	bool LightsRun(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	bool SetParameters(
		ID3D11DeviceContext* devCon, AllofTheLights* _lights, ProjViewMatricies* _viewproj);


	Lights()
	{
		ZeroMemory(this, sizeof(Lights));
	}
	~Lights();
};

