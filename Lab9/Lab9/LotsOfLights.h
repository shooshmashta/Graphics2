#pragma once
#include "Lights.h"

struct LightColor
{
	XMFLOAT4 diffuseColor[NUM_LIGHTS];
};
struct LightPosition
{
	XMFLOAT4 lightPosition[NUM_LIGHTS];
};

struct LotsOfLights
{
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool Init(ID3D11Device*);

	void RenderShader(ID3D11DeviceContext*, int);


	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_lightColBuf;
	ID3D11Buffer* m_lightPosBuf;

public:

	LotsOfLights();
	LotsOfLights(const LotsOfLights&);
	~LotsOfLights();
	PointLights sad;
	bool SetParameters(ID3D11DeviceContext* devCon, LightColor* colors, LightPosition* positions);
	void Shutdown();
	bool Render(ID3D11DeviceContext* devCon, int lightcount, ID3D11PixelShader *pixelShader, ID3D11VertexShader *vertexShader, ID3D11InputLayout *layout);
};

