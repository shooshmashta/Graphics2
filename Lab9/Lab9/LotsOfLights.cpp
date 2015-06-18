#include "LotsOfLights.h"

LotsOfLights::LotsOfLights()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_sampleState = 0;
	m_lightColBuf = 0;
	m_lightPosBuf = 0;
}

LotsOfLights::LotsOfLights(const LotsOfLights& other)
{
}

LotsOfLights::~LotsOfLights()
{
	SAFE_RELEASE(m_vertexShader	);
	SAFE_RELEASE(m_pixelShader	);
	SAFE_RELEASE(m_layout		);
	SAFE_RELEASE(m_sampleState	);
	SAFE_RELEASE(m_lightColBuf	);
	SAFE_RELEASE(m_lightPosBuf	);
}


bool LotsOfLights::Init(ID3D11Device* dev)
{
	HRESULT tester;

	//unsigned int numElements;



	D3D11_BUFFER_DESC lightColBufDesc;
	ZeroMemory(&lightColBufDesc, sizeof(D3D11_BUFFER_DESC));

	lightColBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightColBufDesc.ByteWidth = sizeof(LightColor);
	lightColBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightColBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightColBufDesc.MiscFlags = 0;

	tester = dev->CreateBuffer(&lightColBufDesc, NULL, &m_lightColBuf);

	D3D11_BUFFER_DESC lightPosBufDesc;
	ZeroMemory(&lightPosBufDesc, sizeof(D3D11_BUFFER_DESC));

	lightPosBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightPosBufDesc.ByteWidth = sizeof(LightPosition);
	lightPosBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightPosBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightPosBufDesc.MiscFlags = 0;

	tester = dev->CreateBuffer(&lightPosBufDesc, NULL, &m_lightPosBuf);
	// Create a texture sampler state description.

	
	return true;
}

bool LotsOfLights::SetParameters(ID3D11DeviceContext* devCon, 
	 LightColor* colors, LightPosition* positions)
{
	HRESULT tester;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//
	//unsigned int bufferNumber;

//	tester = devCon->Map(m_lightPosBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(m_lightPosBuf, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, positions, sizeof(LightPosition));

	devCon->Unmap(m_lightPosBuf, NULL);

	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(m_lightColBuf, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, colors, sizeof(LightColor));

	devCon->Unmap(m_lightColBuf, NULL);

	devCon->VSSetConstantBuffers(0, 1, &m_lightPosBuf);
	devCon->PSSetConstantBuffers(0, 1, &m_lightColBuf);

	
	
	return true;
}






