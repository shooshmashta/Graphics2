#pragma once
#include "Defines.h"
struct ProjViewMatricies
{
	XMMATRIX view, proj;
};







struct ObjectModel
{
	
	ObjectModel();

	XMMATRIX world = XMMatrixIdentity();
	bool hasTexture;
	ID3D11Buffer* VertBuff;             // Models vertex buffer
	ID3D11Buffer* IndexBuff;            // Models index buffer
	ID3D11Buffer* matrixLocationBuffer[2];


	ID3D11InputLayout *layout;
	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;

	vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	vector < XMFLOAT3 > v_vertices;
	vector < XMFLOAT2 > v_uvs;
	vector < XMFLOAT3 > v_normals;
	
	ProjViewMatricies * ProjView;

	bool Init(XMFLOAT3 pos,
		ID3D11Device * dev, 
		ID3D11DeviceContext *devCon, 
		ProjViewMatricies* _viewproj,
		bool hasTextures);
	
	bool Run(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);
	
	bool loadOBJ(
		const char * path
		);

	void Cleanup();
};
