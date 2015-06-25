#pragma once
#include "LotsOfLights.h"


struct StrideStruct
{
	XMFLOAT4 m_vect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT2 v_uvs = XMFLOAT2(0.0f, 0.0f);
	XMFLOAT3 v_normals = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT4 v_tangents = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//XMFLOAT3 v_binormals = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

struct Simple_Vert
{
	//position
	XMFLOAT4 m_vect = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	//colors
	XMFLOAT4 m_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

};

struct InstancedObj
{
	XMFLOAT4 m_newPos[4];
};



struct ObjectModel
{
	~ObjectModel();

	XMMATRIX world = XMMatrixIdentity();

	bool hasInstanced = false;
	bool hasTexture = false;
	bool hasLight = false;
	bool hasTrans = false;
	bool isSky = false;

	ID3D11Buffer* VertBuff;             // Models vertex buffer
	ID3D11Buffer* IndexBuff;            // Models index buffer
	ID3D11Buffer* matrixLocationBuffer[2];
	
	//instancing
	ID3D11Buffer* InstanceBuff;
	InstancedObj instances;
	int count;

	ID3D11InputLayout *layout;
	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;
	//all light stuff
	//Lights light;

	//all texture stuff
	ID3D11ShaderResourceView * ObjTexture[3];
	ID3D11SamplerState* ObjTextureSamplerState;
	ID3D11Resource * textureResource[3];

	//blending
	
	ID3D11BlendState* Transparency;
	ID3D11RasterizerState* CCWcullMode;
	ID3D11RasterizerState* CWcullMode;
	

	vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	vector<Simple_Vert> v_vertices;
	vector<StrideStruct> m_stride;
	ProjViewMatricies  *ProjView;


	bool SkyInit(const wchar_t* path,
		ID3D11Device * dev,
		ID3D11DeviceContext *devCon,
		ProjViewMatricies* _viewproj);

	bool LightsInit(XMFLOAT3 pos, 
		const wchar_t* path,
		const wchar_t* norm,
		const wchar_t* spec,
		ID3D11Device * dev,
		ID3D11DeviceContext *devCon,
		ProjViewMatricies* _viewproj, bool trans);

	bool SkyRun(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon,
		ID3D11DepthStencilView * pDSV);

	bool LightsRun(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	bool InstancedRun(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	bool FloorRun(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	bool FlootInit(
		XMFLOAT3 pos,
		const wchar_t* path,
		ID3D11Device * dev,
		ID3D11DeviceContext *devCon,
		ProjViewMatricies* _viewproj);

	bool Init(XMFLOAT3 pos,
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon,
		ProjViewMatricies* _viewproj);

	bool InstancedInit(XMFLOAT3 pos,
		const wchar_t* path,
		const wchar_t* norm,
		ID3D11Device * dev,
		ID3D11DeviceContext *devCon,
		ProjViewMatricies* _viewproj);

	bool Run(
		ID3D11Device* dev,
		ID3D11DeviceContext* devCon);

	bool loadOBJ(
		const char * path
		);

	void ComputeTangents();

};
