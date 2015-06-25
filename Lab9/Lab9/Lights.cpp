#include "Lights.h"


Lights::~Lights()
{
	if (LightBuffer)
	{
	
		SAFE_RELEASE(LightBuffer);
	}
}

bool Lights::Init(XMFLOAT3 pos,
	ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj)
{
	//add pos later
	light.Directional.dir = XMFLOAT3(0.25f, 0.5f, -1.0f);
	light.Directional.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.Directional.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	HRESULT tester;
	D3D11_BUFFER_DESC lightBufDesc;
	ZeroMemory(&lightBufDesc, sizeof(D3D11_BUFFER_DESC));

	lightBufDesc.Usage = D3D11_USAGE_DEFAULT;
	lightBufDesc.ByteWidth = sizeof(light);
	lightBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufDesc.CPUAccessFlags = 0;
	lightBufDesc.MiscFlags = 0;

	tester = dev->CreateBuffer(&lightBufDesc, NULL, &LightBuffer);

	return true;

}

bool Lights::Run(ID3D11Device* dev,
	ID3D11DeviceContext* devCon)
{
	devCon->UpdateSubresource(LightBuffer, 0, NULL, &light, 0, 0);
	devCon->PSSetConstantBuffers(0, 1, &LightBuffer);

	return true;
}


void PointLights::Set(XMFLOAT4 point, XMFLOAT4 _diffuse)
{
	dir = point;
	diffuse = _diffuse;
}

bool Lights::LightsInit(ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj)
{
	//add pos later
	fourlights.Directional.dir = XMFLOAT3(5.0f, 5.0f, 0.0f);
	fourlights.Directional.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	fourlights.Directional.color = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);

	fourlights.Point.position = XMFLOAT3(0, 1, 5);
	fourlights.Point.PointRange = 10.0f;
	fourlights.Point.direction = XMFLOAT4(0, 0, -10, 0);
	fourlights.Point.attentuation = XMFLOAT3(0.0f, 0.1f, 0.0f);
	fourlights.Point.color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	fourlights.Point.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	fourlights.Spot.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	fourlights.Spot.spotRange = 5.0f;
	fourlights.Spot.direction = XMFLOAT3(0, 0, 1.0f);
	fourlights.Spot.attentuation = XMFLOAT3(0.1f, 0.0f, 0.0f);
	fourlights.Spot.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	fourlights.Spot.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	fourlights.Spot.SpotCone = 3;

	On = fourlights.Spot;

	off.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	off.spotRange = 0.0f;
	off.direction = XMFLOAT3(0, 0, 0.0f);
	off.attentuation = XMFLOAT3(0.0f, 0.0f, 0.0f);
	off.color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	off.ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	off.SpotCone = 0;

	HRESULT tester;
	D3D11_BUFFER_DESC lightBufDesc;
	ZeroMemory(&lightBufDesc, sizeof(D3D11_BUFFER_DESC));

	lightBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufDesc.ByteWidth = sizeof(fourlights);
	lightBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufDesc.MiscFlags = 0;
	tester = dev->CreateBuffer(&lightBufDesc, NULL, &LightBuffer);
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(LightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, &fourlights, sizeof(AllofTheLights));

	devCon->Unmap(LightBuffer, NULL);

	devCon->PSSetConstantBuffers(0, 1, &LightBuffer);

	return true;

}


bool Lights::SetParameters(ID3D11DeviceContext* devCon, AllofTheLights* _lights, ProjViewMatricies* _viewproj)
	
{
	HRESULT tester;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	fourlights.Spot.position = XMFLOAT3(_viewproj->view.r[3].m128_f32[0], _viewproj->view.r[3].m128_f32[1], _viewproj->view.r[3].m128_f32[2]);
	fourlights.Spot.direction = XMFLOAT3(_viewproj->view.r[2].m128_f32[0], _viewproj->view.r[2].m128_f32[1], _viewproj->view.r[2].m128_f32[2]);
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(LightBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, &fourlights, sizeof(fourlights));

	devCon->Unmap(LightBuffer, NULL);

	devCon->PSSetConstantBuffers(0, 1, &LightBuffer);



	return true;
}


bool Lights::LightsRun(ID3D11Device* dev,
	ID3D11DeviceContext* devCon)
{
	devCon->UpdateSubresource(LightBuffer, 0, NULL, &light, 0, 0);
	devCon->PSSetConstantBuffers(0, 1, &LightBuffer);
	return true;
}
