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

	unsigned int numElements;


	// Create a texture sampler state description.
	/*D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	tester = dev->CreateSamplerState(&samplerDesc, &m_sampleState);*/

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

	return true;
}

bool LotsOfLights::SetParameters(ID3D11DeviceContext* devCon, 
	 LightColor* colors, LightPosition* positions)
{
	HRESULT tester;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	//
	//unsigned int bufferNumber;

#pragma region garbage?


	//// Transpose the matrices to prepare them for the shader.
	//XMMatrixTranspose(projview->world);
	//XMMatrixTranspose(projview->view);
	//XMMatrixTranspose(projview->proj);

	//// Lock the matrix constant buffer so it can be written to.
	//

	//// Get a pointer to the data in the constant buffer.
	//

	//// Copy the matrices into the constant buffer.
	//dataPtr->world = worldMatrix;
	//dataPtr->view = viewMatrix;
	//dataPtr->projection = projectionMatrix;

	//// Unlock the matrix constant buffer.
	//deviceContext->Unmap(m_matrixBuffer, 0);

	//// Set the position of the matrix constant buffer in the vertex shader.
	//bufferNumber = 0;

	//// Now set the matrix constant buffer in the vertex shader with the updated values.
	//deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);  
#pragma endregion


	tester = devCon->Map(m_lightPosBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);



	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(m_lightPosBuf, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, positions, sizeof(LightPosition));

	devCon->Unmap(m_lightPosBuf, NULL);


	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(m_lightColBuf, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedResource);
	memcpy(mappedResource.pData, colors, sizeof(LightColor));

	devCon->Unmap(m_lightColBuf, NULL);
;

	devCon->VSSetConstantBuffers(1, 1, &m_lightPosBuf);
	devCon->PSSetConstantBuffers(0, 1, &m_lightColBuf);

	return true;
}





bool LotsOfLights::Render(ID3D11DeviceContext* devCon, int lightCount)
{
	// Set the vertex input layout.
	devCon->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	devCon->VSSetShader(m_vertexShader, NULL, 0);
	devCon->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	devCon->PSSetSamplers(0, 1, &m_sampleState);

	// Render the triangle.
	devCon->DrawIndexed(lightCount, 0, 0);

	return true;
}
