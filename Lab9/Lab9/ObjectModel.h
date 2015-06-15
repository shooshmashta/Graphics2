#pragma once
#include "Lights.h"


struct StrideStruct
{
	XMFLOAT4 m_vect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT2 v_uvs = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT3 v_normals = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

struct Simple_Vert
{
	//position
	XMFLOAT4 m_vect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//colors
	XMFLOAT4 m_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);


	//float u, v;
};




struct ObjectModel
{
	
	~ObjectModel();

	XMMATRIX world = XMMatrixIdentity();
	bool hasTexture = false;
	bool hasLight = false;
	ID3D11Buffer* VertBuff;             // Models vertex buffer
	ID3D11Buffer* IndexBuff;            // Models index buffer
	ID3D11Buffer* matrixLocationBuffer[2];

	ID3D11InputLayout *layout;
	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;

	//all light stuff
	Lights light;

	//all texture stuff
	ID3D11ShaderResourceView * ObjTexture;
	ID3D11SamplerState* ObjTextureSamplerState;
	ID3D11Texture2D * textureResource;

	vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	
	vector<Simple_Vert> v_vertices;


	vector<StrideStruct> m_stride;
	
	ProjViewMatricies * ProjView;

	bool Init(XMFLOAT3 pos,
		ID3D11Device * dev, 
		ID3D11DeviceContext *devCon, 
		ProjViewMatricies* _viewproj,
		bool hasTextures, bool hasLights);
	
	bool Run(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	bool RunNewPos(XMFLOAT3 pos,
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);
	
	bool loadOBJ(
		const char * path
		);

	void Cleanup();
};
