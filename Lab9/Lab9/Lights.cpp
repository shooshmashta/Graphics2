#include "Lights.h"

void Lights::Cleanup()
{
	SAFE_RELEASE(LightBuffer);
}


bool Lights::Init(XMFLOAT3 pos,
	ID3D11Device * dev,
	ID3D11DeviceContext *devCon,
	ProjViewMatricies* _viewproj)
{
	//add pos later
	light.Directional.dir = XMFLOAT4(0.25f, 0.5f, -1.0f, 1.0f);
	light.Directional.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	light.Directional.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	light.Point.position = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	light.Point.range = 100.0f;
	light.Point.attentuation = XMFLOAT3(0.0f, 0.2f, 0.0f);
	light.Point.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	light.Point.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);


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