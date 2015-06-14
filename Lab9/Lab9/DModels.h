#pragma once
//#include "Math.h"
#include "Defines.h"


//float DegToRad(float d);


struct TextureManager
{
	std::vector<ID3D11ShaderResourceView*> TextureList;
	std::vector<std::wstring> TextureNameArray;     // So we don't load in the same texture twice
};

// Create material structure
struct SurfaceMaterial
{
	std::wstring MatName;   // So we can match the subset with it's material

	// Surface's colors
	XMFLOAT4 Diffuse;       // Transparency (Alpha) stored in 4th component
	XMFLOAT3 Ambient;
	XMFLOAT4 Specular;      // Specular power stored in 4th component

	// Texture ID's to look up texture in SRV array
	int DiffuseTextureID;
	int AmbientTextureID;
	int SpecularTextureID;
	int AlphaTextureID;
	int NormMapTextureID;

	// Booleans so we don't implement techniques we don't need
	bool HasDiffTexture;
	bool HasAmbientTexture;
	bool HasSpecularTexture;
	bool HasAlphaTexture;
	bool HasNormMap;
	bool IsTransparent;
};

struct Light
{
	Light()
	{
		ZeroMemory(this, sizeof(Light));
	}
	XMFLOAT3 pos;
	float range;
	XMFLOAT3 dir;
	float cone;
	XMFLOAT3 att;
	float pad2;
	XMFLOAT4 ambient;
	XMFLOAT4 diffuse;
};

// Model Structure
struct ObjModel
{
	int Subsets;                        // Number of subsets in obj model
	ID3D11Buffer* VertBuff;             // Models vertex buffer
	ID3D11Buffer* IndexBuff;            // Models index buffer
	std::vector<XMFLOAT3> Vertices;     // Models vertex positions list
	std::vector<DWORD> Indices;         // Models index list
	std::vector<int> SubsetIndexStart;  // Subset's index offset
	std::vector<int> SubsetMaterialID;  // Lookup ID for subsets surface material
	XMMATRIX World;                     // Models world matrix
	std::vector<XMFLOAT3> AABB;			// Stores models AABB (min vertex, max vertex, and center)
	// Where AABB[0] is the min Vertex, and AABB[1] is the max vertex
	XMFLOAT3 Center;					// True center of the model
	float BoundingSphere;				// Model's bounding sphere
};

// Create effects constant buffer's structure //
struct m_cbPerObject
{
	XMMATRIX  WVP;
	XMMATRIX World;

	// These will be used for the pixel shader
	XMFLOAT4 difColor;
	// Because of HLSL structure packing, we will use windows BOOL
	// instead of bool because HLSL packs things into 4 bytes, and
	// bool is only one byte, where BOOL is 4 bytes
	BOOL hasTexture;
	BOOL hasNormMap;
};

struct cbPerFrame
{
	Light  light;
};

struct CameraMatrix
{
	

	XMMATRIX* p_camView;
	XMMATRIX* p_camProjection;
	XMMATRIX* p_camRotationMatrix;


	XMVECTOR m_camPosition;
	XMVECTOR m_camTarget;
	XMVECTOR m_camUp;
	XMVECTOR DefaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR DefaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR camForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR camRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	void Init(XMMATRIX* _camView,
		XMMATRIX* _camProjection,
		XMMATRIX* _camRotationMatrix);
};

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 texCoord;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
	XMFLOAT3 biTangent;
};


struct GeneralHolder
{
	ID3D11RenderTargetView* backBuffer;
	ID3D11Device* d3d11Device;
	IDXGISwapChain* SwapChain;
	ID3D11DeviceContext* d3d11DevCon;
	ID3D11DepthStencilView* depthStencilView;
	DXGI_SWAP_CHAIN_DESC *swapChainDesc;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11RasterizerState* m_RSCullNone;



	void Init(
			ID3D11RenderTargetView* bb,
			ID3D11Device* dev,
			IDXGISwapChain* swap,
			ID3D11DeviceContext* devc,
			ID3D11DepthStencilView* depths,
			ID3D11Texture2D* stencBuf);
};



class DModels
{
	HRESULT* tester;
	XMMATRIX m_WVP;
	GeneralHolder generalHolder;

	//not sure if needed, probably not
	ID3D11VertexShader* m_VS;
	ID3D11PixelShader* m_PS;
	ID3D11InputLayout* m_vertLayout;
	ID3D11Buffer* m_cbPerObjectBuffer;
	ID3D11Buffer* m_cbPerFrameBuffer;
	ID3D11RasterizerState* m_CCWcullMode;
	ID3D11RasterizerState* m_CWcullMode;
	ID3D11SamplerState* m_LinearSamplerState;

	ID3D11Texture2D *m_BackBuffer11;
	IDirectInputDevice8* m_DIKeyboard;
	IDirectInputDevice8* m_DIMouse;
	ID3D11BlendState* m_Transparency;

	

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//ID3D10Blob* VS_Buffer;
	//ID3D10Blob* PS_Buffer;



public:
	DModels();
	~DModels();



	TextureManager m_Textures;
	SurfaceMaterial m_surface;
	Light m_light;
	ObjModel m_objModel;
	m_cbPerObject m_cbPerObj;
	cbPerFrame m_cbPerFrame;
	
	
	CameraMatrix *cam;
	XMMATRIX m_camRotationMatrix;

	//initialize 
	void Init(
		GeneralHolder gh,
		CameraMatrix *_cam,
		HRESULT *_tester
		);


	void DrawScene();

	//std::vector<SurfaceMaterial> material;


	//Define LoadObjModel function after we create surfaceMaterial structure
	bool LoadObjModel(
		std::wstring Filename,                      // obj model filename (model.obj)
		bool IsRHCoordSys,                          // True if model was created in right hand coord system
		bool ComputeNormals,						  // True to compute the normals, false to use the files normals
		bool hasMaterials);							//TRUE IS HAS MATS	   

};
