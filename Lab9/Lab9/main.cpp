// D3D LAB 1a "Line Land".
// Author: L.Norri CD DRX, FullSail University

// Introduction:
// Welcome to the first lab of the Direct3D Graphics Programming class.
// This is the ONLY guided lab in this course! 
// Future labs will demand the habits & foundation you develop right now!  
// It is CRITICAL that you follow each and every step. ESPECIALLY THE READING!!!

// TO BEGIN: Open the word document that acompanies this lab and start from the top.

//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************


//#include <d3dx11.h>

#include "Math.h"


// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "d3dx11.lib")


#define SAFE_RELEASE(p) {if(p){p->Release(); p=nullptr;}}


using namespace std;



// TODO: PART 2 STEP 6
//in defines


//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	// TODO: PART 1 STEP 2
	ID3D11RenderTargetView			*backBuffer;

	ID3D11Device					*dev;
	IDXGISwapChain					*swap;
	ID3D11DeviceContext				*devCon;

	DXGI_SWAP_CHAIN_DESC			scd;
	D3D11_VIEWPORT					viewport;

	HRESULT tester;
	// TODO: PART 2 STEP 2
	ID3D11RasterizerState * rasterStateSolid;
	ID3D11RasterizerState * rasterStateWire;

	Simple_Vertex4 star[12];
	Simple_Vertex4 triangle[3];
	ID3D11Buffer *triangleBuf;
	ID3D11Buffer *indexBuf;
	XMMATRIX star1World = XMMatrixIdentity();

	bool wireOn = false;

	ID3D11InputLayout *layout;

	TriIndexBuffer starIndex[20];

	// BEGIN PART 5
	// TODO: PART 5 STEP 1

	// TODO: PART 2 STEP 4

	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;

	ID3D11DepthStencilView* pDSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ID3D11Texture2D* pDepthStencil = NULL;
	// BEGIN PART 3
	// TODO: PART 3 STEP 1

	ID3D11Buffer *newBuf[2];
	XTime timething;

	ObjectModel pyramid;

	ProjViewMatricies OotherM;

	struct SEND_TO_VRAM
	{
		XMFLOAT4 point;

	};
	float rotate;

	// TODO: PART 3 STEP 4a

	SEND_TO_VRAM toShader;
	//otherMatrixes otherM;



public:
	// BEGIN PART 2
	// TODO: PART 2 STEP 1

	//structure went here

	DEMO_APP(HINSTANCE hinst, WNDPROC proc);
	bool Run();
	bool ShutDown();
};




//************************************************************
//************ CREATION OF OBJECTS & RESOURCES ***************
//************************************************************

DEMO_APP::DEMO_APP(HINSTANCE hinst, WNDPROC proc)
{
	XMMATRIX _m = XMMatrixIdentity();
	_m = XMMatrixTranslation(0, 0, 4);
	star1World = _m + star1World;
	
	ProjPerspectiveMatrixInit();
	XMVECTOR mattemp;
	mattemp.m128_f32[0] = (ProjPerspectiveMatrix.vert[0][0]);
	mattemp.m128_f32[1] = (ProjPerspectiveMatrix.vert[0][1]);
	mattemp.m128_f32[2] = (ProjPerspectiveMatrix.vert[0][2]);
	mattemp.m128_f32[3] = (ProjPerspectiveMatrix.vert[0][3]);
	
	OotherM.proj.r[0] = mattemp;

	mattemp.m128_f32[0] = (ProjPerspectiveMatrix.vert[1][0]);
	mattemp.m128_f32[1] = (ProjPerspectiveMatrix.vert[1][1]);
	mattemp.m128_f32[2] = (ProjPerspectiveMatrix.vert[1][2]);
	mattemp.m128_f32[3] = (ProjPerspectiveMatrix.vert[1][3]);

	OotherM.proj.r[1] = mattemp;

	mattemp.m128_f32[0] = (ProjPerspectiveMatrix.vert[2][0]);
	mattemp.m128_f32[1] = (ProjPerspectiveMatrix.vert[2][1]);
	mattemp.m128_f32[2] = (ProjPerspectiveMatrix.vert[2][2]);
	mattemp.m128_f32[3] = (ProjPerspectiveMatrix.vert[2][3]);

	OotherM.proj.r[2] = mattemp;

	mattemp.m128_f32[0] = (ProjPerspectiveMatrix.vert[3][0]);
	mattemp.m128_f32[1] = (ProjPerspectiveMatrix.vert[3][1]);
	mattemp.m128_f32[2] = (ProjPerspectiveMatrix.vert[3][2]);
	mattemp.m128_f32[3] = (ProjPerspectiveMatrix.vert[3][3]);

	OotherM.proj.r[3] = mattemp;
	
	OotherM.view =  XMMatrixIdentity();

	
	rotate = 0;

	//timething.Signal();

	bool everyother = true;

	for (int i = 0; i < 10; i++)
	{

		if (everyother)
		{
			everyother = false;
			star[i].x = cosf(DegToRad(36.0f * (float)(i)));
			star[i].y = sinf(DegToRad(36.0f * (float)(i)));
		}
		else
		{
			star[i].x = cosf(DegToRad(36.0f * (float)i)) / 2;
			star[i].y = sinf(DegToRad(36.0f * (float)i)) / 2;
			everyother = true;
		}

		star[i].r = 0.5f;
		star[i].g = 0.25f;

	}

	star[10].z = 0.3f;
	star[11].z = -0.3f;

	star[10].r = 1;
	star[10].g = 1;
	star[11].r = 1;
	star[11].g = 1;


	


	// ****************** BEGIN WARNING ***********************// 
	// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY! 
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//

	// TODO: PART 1 STEP 3a
	//swap chain settings
	scd.BufferCount = 1;                                    // one back buffer

	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferDesc.Width = BACKBUFFER_WIDTH;
	scd.BufferDesc.Height = BACKBUFFER_HEIGHT;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.RefreshRate.Numerator = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;// | D3D11_CREATE_DEVICE_DEBUG;
	scd.OutputWindow = window;                              // the window to be used
	scd.SampleDesc.Count = 1;                               // how many multisamples
	scd.SampleDesc.Quality = 0;
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;


	const UINT num_requested_feature_levels = 1;
	// TODO: PART 1 STEP 3b
	tester = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION,
		&scd, &swap, &dev, NULL, &devCon);

	// TODO: PART 1 STEP 4

	////get the address of the back buffer
	ID3D11Texture2D *_BackBuffer = nullptr;
	tester = swap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_BackBuffer));
	//
	//use the back buffer address to create the render target
	tester = dev->CreateRenderTargetView(_BackBuffer, NULL, &backBuffer);
	_BackBuffer->Release();

	//set the render target as the back buffer
	devCon->OMSetRenderTargets(1, &backBuffer, NULL);

	
	// TODO: PART 1 STEP 5
	//viewport
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = BACKBUFFER_WIDTH;
	viewport.Height = BACKBUFFER_HEIGHT;

	//set viewport
	devCon->RSSetViewports(1, &viewport);



	// TODO: PART 2 STEP 3b
	D3D11_BUFFER_DESC assigner;
	assigner.Usage = D3D11_USAGE_IMMUTABLE;
	assigner.CPUAccessFlags = NULL;
	assigner.MiscFlags = NULL;
	assigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	assigner.ByteWidth = sizeof(Simple_Vertex4) * 12;
	assigner.StructureByteStride = 0;


	
	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA sub;
	sub.pSysMem = star;
	sub.SysMemPitch = 0;
	sub.SysMemSlicePitch = 0;
	// TODO: PART 2 STEP 3d
	tester = dev->CreateBuffer(&assigner, &sub, &triangleBuf);
	tester = dev->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pixelShader);
	tester = dev->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vertexShader);
	// TODO: PART 2 STEP 8a


	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	tester = dev->CreateInputLayout(vLayout, 2, Trivial_VS, sizeof(Trivial_VS), &layout);
	// TODO: PART 3 STEP 3

	D3D11_BUFFER_DESC newbuf[2];
	newbuf[0].ByteWidth = sizeof(Matrix4x4);
	newbuf[0].Usage = D3D11_USAGE_DYNAMIC;
	newbuf[0].BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	newbuf[0].CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	newbuf[0].MiscFlags = NULL;


	newbuf[1].ByteWidth = sizeof(OotherM);
	newbuf[1].Usage = D3D11_USAGE_DYNAMIC;
	newbuf[1].BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	newbuf[1].CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	newbuf[1].MiscFlags = NULL;
	//newbuf.StructureByteStride = 0;


	
	//index buffer settings
	D3D11_BUFFER_DESC indexBuffer;
	indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
	indexBuffer.CPUAccessFlags = NULL;
	indexBuffer.MiscFlags = NULL;
	indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBuffer.ByteWidth = sizeof(TriIndexBuffer) * 20;
	indexBuffer.StructureByteStride = 0;
	
	for (int i = 0; i < 10; i++)
	{
		starIndex[i].i1 = i;
		starIndex[i].i2 = i + 1;

		starIndex[i + 10].i1 = i + 1;
		starIndex[i + 10].i2 = i;
		starIndex[i + 10].i0 = 11;

		if (i > 8)
		{
			starIndex[i].i2 = 0;

			starIndex[i + 10].i1 = 0;
			starIndex[i + 10].i2 = i;
		}
	}

	D3D11_SUBRESOURCE_DATA indexSub;
	indexSub.pSysMem = starIndex;
	indexSub.SysMemPitch = 0;
	indexSub.SysMemSlicePitch = 0;
	
	tester = dev->CreateBuffer(&newbuf[0], NULL, &newBuf[0]);
	tester = dev->CreateBuffer(&newbuf[1], NULL, &newBuf[1]);
	tester = dev->CreateBuffer(&indexBuffer, &indexSub, &indexBuf);


	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.MiscFlags = NULL;
	descDepth.CPUAccessFlags = NULL;
	descDepth.SampleDesc.Quality = 0;
	descDepth.SampleDesc.Count = 1;
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	//descDepth.
	tester = dev->CreateTexture2D(&descDepth, NULL, &pDepthStencil);
	descDepth.SampleDesc.Count = 1;


	D3D11_DEPTH_STENCIL_DESC dsDesc;
	
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0;
	dsDesc.StencilWriteMask = 0;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;


	ZeroMemory(&pDSV, sizeof(ID3D11DepthStencilView));

	// Create depth stencil state
	ID3D11DepthStencilState * pDSState;
	tester = dev->CreateDepthStencilState(&dsDesc, &pDSState);

	devCon->OMSetDepthStencilState(pDSState, 1);


	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	descDSV.Flags = NULL;

	tester = dev->CreateDepthStencilView(pDepthStencil, // Depth stencil texture
		&descDSV, // Depth stencil desc
		&pDSV);  // [out] Depth stencil view
	//stencilviews


	D3D11_RASTERIZER_DESC rState;
	ZeroMemory(&rState, sizeof(D3D11_RASTERIZER_DESC));
	rState.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rState.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	rState.DepthClipEnable = true;
	rState.AntialiasedLineEnable = true;
	rState.MultisampleEnable = true;

	dev->CreateRasterizerState(&rState, &rasterStateSolid);

	//ZeroMemory(&rState, sizeof(D3D11_RASTERIZER_DESC));
	rState.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rState.AntialiasedLineEnable = false;
	rState.MultisampleEnable = false;

	dev->CreateRasterizerState(&rState, &rasterStateWire);
	devCon->RSSetState(rasterStateSolid);
	pDSState->Release();
	


	pyramid.loadOBJ("pyramid1.obj");
	pyramid.Init(XMFLOAT3(1, 1, 1), dev, devCon, &OotherM, false);






}
//****************************************************************************
//**************************** EXECUTION *************************************
//****************************************************************************

bool DEMO_APP::Run()
{
#pragma region Movement

	if ((GetAsyncKeyState('I') & 0x1))
	{
		if (wireOn)
		{
			wireOn = !wireOn;
			devCon->RSSetState(rasterStateSolid);
		}
		else 
		{
			wireOn = !wireOn;
			devCon->RSSetState(rasterStateWire);
		}
	}
	
	Vertex4 viewMovement;
	if (GetAsyncKeyState('W'))
	{
		viewMovement.vert[0] = 0;
		viewMovement.vert[1] = 0;
		viewMovement.vert[2] = 0.01f;

		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.vert[0], viewMovement.vert[1], viewMovement.vert[2]);
		OotherM.view = _m * OotherM.view;


	}
	if (GetAsyncKeyState('A'))
	{
		viewMovement.vert[0] = -0.01f;
		viewMovement.vert[1] = 0;
		viewMovement.vert[2] = 0.0f;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.vert[0], viewMovement.vert[1], viewMovement.vert[2]);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('S'))
	{
		viewMovement.vert[0] = 0;
		viewMovement.vert[1] = 0;
		viewMovement.vert[2] = -0.01f;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.vert[0], viewMovement.vert[1], viewMovement.vert[2]);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('D'))
	{

		viewMovement.vert[0] = 0.01f;
		viewMovement.vert[1] = 0;
		viewMovement.vert[2] = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.vert[0], viewMovement.vert[1], viewMovement.vert[2]);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('R'))
	{

		viewMovement.vert[0] = 0;
		viewMovement.vert[1] = 0.01f;
		viewMovement.vert[2] = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.vert[0], viewMovement.vert[1], viewMovement.vert[2]);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('F'))
	{
		viewMovement.vert[0] = 0;
		viewMovement.vert[1] = -0.01f;
		viewMovement.vert[2] = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.vert[0], viewMovement.vert[1], viewMovement.vert[2]);
		OotherM.view = _m * OotherM.view;
	}


	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		XMMATRIX _m = XMMatrixIdentity();
		_m = _m * XMMatrixRotationY(DegToRad(0.1f));


		float _x = OotherM.view.r[3].m128_f32[0];
		float _y = OotherM.view.r[3].m128_f32[1];
		float _z = OotherM.view.r[3].m128_f32[2];
		OotherM.view.r[3].m128_f32[0] = 0;
		OotherM.view.r[3].m128_f32[1] = 0;
		OotherM.view.r[3].m128_f32[2] = 0;

		OotherM.view = _m * OotherM.view;

		OotherM.view.r[3].m128_f32[0] = _x;
		OotherM.view.r[3].m128_f32[1] = _y;
		OotherM.view.r[3].m128_f32[2] = _z;

	}
	else if (GetAsyncKeyState(VK_NUMPAD6))
	{
		XMMATRIX _m = XMMatrixIdentity();
		_m = _m * XMMatrixRotationY(DegToRad(-0.1f));

		float _x = OotherM.view.r[3].m128_f32[0];
		float _y = OotherM.view.r[3].m128_f32[1];
		float _z = OotherM.view.r[3].m128_f32[2];
		OotherM.view.r[3].m128_f32[0] = 0;
		OotherM.view.r[3].m128_f32[1] = 0;
		OotherM.view.r[3].m128_f32[2] = 0;

		OotherM.view = _m * OotherM.view;

		OotherM.view.r[3].m128_f32[0] = _x;
		OotherM.view.r[3].m128_f32[1] = _y;
		OotherM.view.r[3].m128_f32[2] = _z;
	}

	if (GetAsyncKeyState(VK_NUMPAD5))
	{
		OotherM.view = OotherM.view * XMMatrixRotationX(DegToRad(-0.1f));
	}
	else if (GetAsyncKeyState(VK_NUMPAD8))
	{
		OotherM.view = OotherM.view * XMMatrixRotationX(DegToRad(0.1f));
	}
#pragma endregion

#pragma region Buffer Clearing

	devCon->OMSetRenderTargets(1, &backBuffer, pDSV);

	devCon->RSSetViewports(1, &viewport);

	FLOAT f[4];
	//rgba
	f[0] = 0; f[1] = 0; f[2] = 0.4f; f[3] = 0;

	devCon->ClearRenderTargetView(backBuffer, f);
	devCon->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1, 0);
#pragma endregion

#pragma region Star 1
	float _z = star1World.r[3].m128_f32[2];
	star1World.r[3].m128_f32[2] = 0;
	star1World = star1World * XMMatrixRotationY(rotate);
	star1World.r[3].m128_f32[2] = _z;

	rotate =  (float)timething.Delta();
	//star1World.vert[3][2] = 2;

	timething.Signal();
	OotherM.view = XMMatrixInverse(nullptr, OotherM.view);
	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(newBuf[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, &star1World, sizeof(star1World));

	devCon->Unmap(newBuf[0], NULL);

	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(newBuf[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, &OotherM, sizeof(OotherM));
	devCon->Unmap(newBuf[1], NULL);
	OotherM.view = XMMatrixInverse(nullptr, OotherM.view);
	// TODO: PART 3 STEP 6

	devCon->VSSetConstantBuffers(0, 2, newBuf);

	// TODO: PART 2 STEP 9a
	UINT strides = sizeof(Simple_Vertex4);
	UINT offsets = 0;

	devCon->IASetVertexBuffers(0, 1, &triangleBuf, &strides, &offsets);

	devCon->IASetIndexBuffer(indexBuf, DXGI_FORMAT_R16_UINT, offsets);
	// TODO: PART 2 STEP 9b
	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	// TODO: PART 2 STEP 9c
	devCon->IASetInputLayout(layout);

	// TODO: PART 2 STEP 9d
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: PART 2 STEP 10
	devCon->DrawIndexed(60, 0, 0);  
#pragma endregion

#pragma region Star 2

	pyramid.Run(dev, devCon);


#pragma endregion



	tester = swap->Present(0, 0);

	// END OF PART 1
	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	pyramid.Cleanup();
	// TODO: PART 1 STEP 6
	SAFE_RELEASE(devCon);
	SAFE_RELEASE(dev);
	SAFE_RELEASE(backBuffer);
	SAFE_RELEASE(swap);
	SAFE_RELEASE(layout);
	SAFE_RELEASE(triangleBuf);
	SAFE_RELEASE(indexBuf);
	SAFE_RELEASE(pDSV);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(newBuf[0]);
	SAFE_RELEASE(newBuf[1]);
	SAFE_RELEASE(pDepthStencil);
	SAFE_RELEASE(rasterStateSolid);
	SAFE_RELEASE(rasterStateWire);

	UnregisterClass(L"DirectXApplication", application);
	return true;
}

//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************

// ****************** BEGIN WARNING ***********************// 
// WINDOWS CODE, I DON'T TEACH THIS YOU MUST KNOW IT ALREADY!

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	DEMO_APP myApp(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && myApp.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	myApp.ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
						break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//