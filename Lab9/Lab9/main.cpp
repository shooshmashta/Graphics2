//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include "Math.h"



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
	DXGI_SWAP_CHAIN_DESC			scdFullScreen;
	D3D11_VIEWPORT					viewport;

	HRESULT tester;
	// TODO: PART 2 STEP 2
	ID3D11RasterizerState * rasterStateSolid;
	ID3D11RasterizerState * rasterStateWire;

	Simple_Vertex4 star[12];
	Simple_Vertex4 triangle[3];
	ID3D11Buffer *CubetriangleBuf;
	ID3D11Buffer *indexBuf;
	Matrix4x4 star1World = WorldMatrixInit();
	bool wireOn = false;

	//grid
	Simple_Vertex4 grid[80];
	Matrix4x4 gridWorld = WorldMatrixInit();
	int2 gridIndicies[40];

	ID3D11Buffer *GridtriangleBuf;
	ID3D11PixelShader *gridPixelShader;
	ID3D11VertexShader *gridVertexShader;
	ID3D11InputLayout *gridLayout;
	ID3D11Buffer *gridIndexBuf;

	ID3D11Buffer *gridBuff;







	ID3D11InputLayout *layout;
	bool starOnScreen = false;
	TriIndexBuffer starIndex[20];

	// BEGIN PART 5
	// TODO: PART 5 STEP 1
	LONG currMouseY = 0, lastmouseY = 0;
	LONG currMouseX = 0, lastmouseX = 0;
	// TODO: PART 2 STEP 4

	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;

	ID3D11SamplerState *samplerState;
	ID3D11ShaderResourceView *shaderRes;

	ID3D11DepthStencilView* pDSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ID3D11Texture2D* pDepthStencil = NULL;
	ID3D11Texture2D* pTexture2D = NULL;
	// BEGIN PART 3
	// TODO: PART 3 STEP 1

	ID3D11Buffer *newBuf[2];
	XTime timething;


	struct SEND_TO_VRAM
	{
		Simple_Vertex4 point;

	};
	float rotate;

	// TODO: PART 3 STEP 4a

	SEND_TO_VRAM toShader;
	otherMatrixes otherM;



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
	gridWorld = star1World = WorldMatrixInit();
	ViewMatrixInit();
	ProjPerspectiveMatrixInit();


	otherM.proj = ProjPerspectiveMatrix;
	otherM.view = ViewMatrix;
	rotate = 0;

	bool everyother = true;
	//the star is alive
#pragma region Star Defines
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

#pragma endregion

	
#pragma region Grid Init
	for (int i = 0; i < 20; i++)
	{
		grid[i * 2].x = (i * 0.2f) - 2;
		grid[(i * 2) + 1].x = (i * 0.2f) - 2;

		grid[i * 2].y = 0;
		grid[(i * 2) + 1].y = 0;

		grid[i * 2].z = -2;
		grid[(i * 2) + 1].z = 2;

	/*	grid[i * 2].r = 0;
		grid[i * 2].g = 1;
		grid[i * 2].b = 0;
		grid[i * 2].a = 0;
		grid[(i * 2) + 1].r = 0;
		grid[(i * 2) + 1].g = 1;
		grid[(i * 2) + 1].b = 0;
		grid[(i * 2) + 1].a = 0;

		grid[40 + (2 * i)].r = 0;
		grid[40 + (2 * i)].g = 1;
		grid[40 + (2 * i)].b = 0;
		grid[40 + (2 * i)].a = 0;
		grid[40 + (2 * i) + 1].r = 0;
		grid[40 + (2 * i) + 1].g = 1;
		grid[40 + (2 * i) + 1].b = 0;
		grid[40 + (2 * i) + 1].a = 0;*/



		grid[(i * 2) + 40].x = 2;
		grid[(i * 2) + 41].x = -2;

		grid[(i * 2) + 40].y = 0;
		grid[(i * 2) + 41].y = 0;

		grid[(i * 2) + 40].z = (i * 0.2f) - 2;
		grid[(i * 2) + 41].z = (i * 0.2f) - 2;
	}

	


#pragma endregion



#pragma region window
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

#pragma endregion

#pragma region Inits
	// TODO: PART 1 STEP 3a
#pragma region SwapChainWindow
	//swap chain settings
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));


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
#pragma endregion








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



	
#pragma endregion


#pragma region CubeSubresource

	//if (starOnScreen)
	// TODO: PART 2 STEP 3b
	D3D11_BUFFER_DESC assigner;
	assigner.Usage = D3D11_USAGE_IMMUTABLE;
	assigner.CPUAccessFlags = NULL;
	assigner.MiscFlags = NULL;
	assigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	assigner.ByteWidth = sizeof(_OBJ_VERT_) * 776;
	assigner.StructureByteStride = 0;

	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA cubeSub;
	cubeSub.pSysMem = Cube_data;
	cubeSub.SysMemPitch = 0;
	cubeSub.SysMemSlicePitch = 0;
	// TODO: PART 2 STEP 3d
	tester = dev->CreateBuffer(&assigner, &cubeSub, &CubetriangleBuf);


#pragma endregion

#pragma region Grid SubRescource
	D3D11_BUFFER_DESC gridAssigner;
	gridAssigner.Usage = D3D11_USAGE_IMMUTABLE;
	gridAssigner.CPUAccessFlags = NULL;
	gridAssigner.MiscFlags = NULL;
	gridAssigner.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	gridAssigner.ByteWidth = sizeof(Simple_Vertex4) * GRIDSIZE;
	gridAssigner.StructureByteStride = 0;

	// TODO: PART 2 STEP 3c
	D3D11_SUBRESOURCE_DATA gridSub;
	gridSub.pSysMem = grid;
	gridSub.SysMemPitch = 0;
	gridSub.SysMemSlicePitch = 0;
	
	
	// TODO: PART 2 STEP 3d
	tester = dev->CreateBuffer(&gridAssigner, &gridSub, &GridtriangleBuf);

#pragma endregion



#pragma region CubeShaders
	// TODO: PART 2 STEP 7
	tester = dev->CreatePixelShader(Trivial_PS, sizeof(Trivial_PS), NULL, &pixelShader);
	tester = dev->CreateVertexShader(Trivial_VS, sizeof(Trivial_VS), NULL, &vertexShader);
	// TODO: PART 2 STEP 8a


	D3D11_INPUT_ELEMENT_DESC vLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC pLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// TODO: PART 2 STEP 8b

	

	tester = dev->CreateInputLayout(vLayout, 3, Trivial_VS, sizeof(Trivial_VS), &layout);
#pragma endregion

#pragma region GridShaders

	// TODO: PART 2 STEP 7
	tester = dev->CreatePixelShader(Grid_PS, sizeof(Grid_PS), NULL, &gridPixelShader);
	tester = dev->CreateVertexShader(Grid_VS, sizeof(Grid_VS), NULL, &gridVertexShader);
	// TODO: PART 2 STEP 8a


	D3D11_INPUT_ELEMENT_DESC cvLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// TODO: PART 2 STEP 8b

	tester = dev->CreateInputLayout(cvLayout, 2, Grid_VS, sizeof(Grid_VS), &gridLayout);

#pragma endregion


#pragma region Cube Textures
	// TODO: PART 3 STEP 3

	D3D11_BUFFER_DESC newbuf[2];
	newbuf[0].ByteWidth = sizeof(Matrix4x4);
	newbuf[0].Usage = D3D11_USAGE_DYNAMIC;
	newbuf[0].BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	newbuf[0].CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	newbuf[0].MiscFlags = NULL;

	newbuf[1].ByteWidth = sizeof(otherM);
	newbuf[1].Usage = D3D11_USAGE_DYNAMIC;
	newbuf[1].BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	newbuf[1].CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	newbuf[1].MiscFlags = NULL;
	
	tester = dev->CreateBuffer(&newbuf[0], NULL, &newBuf[0]);
	tester = dev->CreateBuffer(&newbuf[1], NULL, &newBuf[1]);

	//TextureBuffer
	D3D11_TEXTURE2D_DESC descText;
	descText.MipLevels = Tron_numlevels;
	descText.ArraySize = 1;
	descText.MiscFlags = NULL;
	descText.CPUAccessFlags = NULL;
	descText.SampleDesc.Quality = 0;
	descText.SampleDesc.Count = 1;
	descText.Width = Tron_width;
	descText.Height = Tron_height;
	descText.Usage = D3D11_USAGE_DEFAULT;
	descText.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	descText.BindFlags = D3D11_BIND_SHADER_RESOURCE;



	D3D11_SUBRESOURCE_DATA textSub[Tron_numlevels];
	for (int i = 0; i < Tron_numlevels; i++)
	{
		ZeroMemory(&textSub[i], sizeof(textSub[i]));
		textSub[i].pSysMem = Tron_pixels + Tron_leveloffsets[i];
		textSub[i].SysMemPitch = (Tron_width >> i) * sizeof(unsigned int);
	}

	tester = dev->CreateTexture2D(&descText, textSub, &pTexture2D);

	D3D11_SAMPLER_DESC sampleDec;
	sampleDec.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDec.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDec.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDec.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampleDec.MinLOD = (-FLT_MAX);
	sampleDec.MaxLOD = (FLT_MAX);
	sampleDec.MipLODBias = 0.0f;
	sampleDec.MaxAnisotropy = 1;
	sampleDec.ComparisonFunc = D3D11_COMPARISON_NEVER;


	tester = dev->CreateSamplerState(&sampleDec, &samplerState);

	D3D11_SHADER_RESOURCE_VIEW_DESC pDesc;
	pDesc.Texture2D.MipLevels = Tron_numlevels;
	pDesc.Texture2D.MostDetailedMip = 0;
	pDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	pDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;


	D3D11_BUFFEREX_SRV pShaderBuf;
	pShaderBuf.FirstElement = 0;
	pShaderBuf.NumElements = Tron_numpixels;
	pShaderBuf.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	pDesc.BufferEx = pShaderBuf;


	tester = dev->CreateShaderResourceView(pTexture2D, NULL, &shaderRes);
	devCon->PSSetShaderResources(0, 1, &shaderRes);
#pragma endregion




#pragma region Cube Index
	//if (starOnScreen)
	//{
		//index buffer settings
		D3D11_BUFFER_DESC indexBuffer;
		indexBuffer.Usage = D3D11_USAGE_IMMUTABLE;
		indexBuffer.CPUAccessFlags = NULL;
		indexBuffer.MiscFlags = NULL;
		indexBuffer.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBuffer.ByteWidth = sizeof(unsigned int) * Cube_index_size;
		indexBuffer.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexSub;
		indexSub.pSysMem = Cube_indicies;
		indexSub.SysMemPitch = 0;
		indexSub.SysMemSlicePitch = 0;
		
		
		
		tester = dev->CreateBuffer(&indexBuffer, &indexSub, &indexBuf);

	//}
#pragma endregion



#pragma region Grid Index


		//if (starOnScreen)
		//{
		//index buffer settings
		D3D11_BUFFER_DESC gridIndex;
		gridIndex.Usage = D3D11_USAGE_IMMUTABLE;
		gridIndex.CPUAccessFlags = NULL;
		gridIndex.MiscFlags = NULL;
		gridIndex.BindFlags = D3D11_BIND_INDEX_BUFFER;
		gridIndex.ByteWidth = sizeof(float2) * 40;
		gridIndex.StructureByteStride = 0;


		for (int i = 0; i < 20; i++)
		{
			gridIndicies[(i * 2)].x = (i * 2);
			gridIndicies[(i * 2) + 1].y = (i * 2) + 1;

		}
	
		D3D11_SUBRESOURCE_DATA gridIndexSub;
		gridIndexSub.pSysMem = gridIndicies;
		gridIndexSub.SysMemPitch = 0;
		gridIndexSub.SysMemSlicePitch = 0;

		tester = dev->CreateBuffer(&newbuf[0], NULL, &gridBuff);
		//tester = dev->CreateBuffer(&newbuf[1], NULL, &gridBuff[1]);
		tester = dev->CreateBuffer(&gridIndex, &gridIndexSub, &gridIndexBuf);

		
#pragma endregion

		lastmouseY = 0;

#pragma region Cube Depth Stencil
		//DepthBuffer
		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.MipLevels = Tron_numlevels;
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


#pragma endregion

#pragma region Wireframe Mode
		D3D11_RASTERIZER_DESC rState;
		ZeroMemory(&rState, sizeof(D3D11_RASTERIZER_DESC));
		rState.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rState.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rState.DepthClipEnable = true;
		rState.AntialiasedLineEnable = true;
		rState.MultisampleEnable = true;
		dev->CreateRasterizerState(&rState, &rasterStateSolid);

		//rState.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		rState.AntialiasedLineEnable = false;
		rState.MultisampleEnable = false;
		dev->CreateRasterizerState(&rState, &rasterStateWire);
		pDSState->Release();
#pragma endregion
		devCon->RSSetState(rasterStateSolid);
}
//****************************************************************************
//**************************** EXECUTION *************************************
//****************************************************************************

bool DEMO_APP::Run()
{
#pragma region Movement

#pragma region Keyboard
	if (GetAsyncKeyState('O') & 0x1)
	{
		starOnScreen = !starOnScreen;
	}

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

		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixTranslate(_m, viewMovement);

		otherM.view = Matrix4x4Multiply(_m, otherM.view);

	}
	if (GetAsyncKeyState('A'))
	{
		viewMovement.vert[0] = -0.01f;
		viewMovement.vert[1] = 0;
		viewMovement.vert[2] = 0.0f;
		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixTranslate(_m, viewMovement);


		otherM.view = Matrix4x4Multiply(_m, otherM.view);
	}
	if (GetAsyncKeyState('S'))
	{
		viewMovement.vert[0] = 0;
		viewMovement.vert[1] = 0;
		viewMovement.vert[2] = -0.01f;
		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixTranslate(_m, viewMovement);


		otherM.view = Matrix4x4Multiply(_m, otherM.view);
	}
	if (GetAsyncKeyState('D'))
	{

		viewMovement.vert[0] = 0.01f;
		viewMovement.vert[1] = 0;
		viewMovement.vert[2] = 0;
		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixTranslate(_m, viewMovement);


		otherM.view = Matrix4x4Multiply(_m, otherM.view);
	}
	if (GetAsyncKeyState('R'))
	{

		viewMovement.vert[0] = 0;
		viewMovement.vert[1] = 0.01f;
		viewMovement.vert[2] = 0;
		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixTranslate(_m, viewMovement);


		otherM.view = Matrix4x4Multiply(_m, otherM.view);
	}
	if (GetAsyncKeyState('F'))
	{
		viewMovement.vert[0] = 0;
		viewMovement.vert[1] = -0.01f;
		viewMovement.vert[2] = 0;
		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixTranslate(_m, viewMovement);


		otherM.view = Matrix4x4Multiply(_m, otherM.view);
	}


	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixRotateYDeg(_m, DegToRad(0.1f));

		float _x = otherM.view.vert[3][0];
		float _y = otherM.view.vert[3][1];
		float _z = otherM.view.vert[3][2];
		otherM.view.vert[3][0] = 0;
		otherM.view.vert[3][1] = 0;
		otherM.view.vert[3][2] = 0;

		otherM.view = Matrix4x4Multiply(_m, otherM.view);

		otherM.view.vert[3][0] = _x;
		otherM.view.vert[3][1] = _y;
		otherM.view.vert[3][2] = _z;

	}
	else if (GetAsyncKeyState(VK_NUMPAD6))
	{
		Matrix4x4 _m = IdentityMatrix();
		_m = MatrixRotateYDeg(_m, DegToRad(-0.1f));

		float _x = otherM.view.vert[3][0];
		float _y = otherM.view.vert[3][1];
		float _z = otherM.view.vert[3][2];
		otherM.view.vert[3][1] = 0;
		otherM.view.vert[3][0] = 0;
		otherM.view.vert[3][2] = 0;

		otherM.view = Matrix4x4Multiply(_m, otherM.view);

		otherM.view.vert[3][0] = _x;
		otherM.view.vert[3][1] = _y;
		otherM.view.vert[3][2] = _z;
	}

	if (GetAsyncKeyState(VK_NUMPAD5))
	{
		otherM.view = MatrixRotateXDeg(otherM.view, DegToRad(-0.1f));
	}
	else if (GetAsyncKeyState(VK_NUMPAD8))
	{
		otherM.view = MatrixRotateXDeg(otherM.view, DegToRad(0.1f));
	}
#pragma endregion

#pragma region Mouse 
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		POINT point;
		if (GetCursorPos(&point))
		{
			currMouseX = point.x;
			currMouseY = point.y;

			if (lastmouseX == 0 && lastmouseY == 0)
			{
				lastmouseX = currMouseX;
				lastmouseY = currMouseY;
			}

			int differenceX = currMouseX - lastmouseX;
			int differenceY = currMouseY - lastmouseY;

			lastmouseX = currMouseX;
			lastmouseY = currMouseY;

			float _x = otherM.view.vert[3][0];
			float _y = otherM.view.vert[3][1];
			float _z = otherM.view.vert[3][2];
			otherM.view.vert[3][0] = 0;
			otherM.view.vert[3][1] = 0;
			otherM.view.vert[3][2] = 0;

			Matrix4x4 _m = IdentityMatrix();

			_m = MatrixRotateYDeg(_m, DegToRad(differenceX * 0.2f));
			_m = MatrixRotateXDeg(_m, DegToRad(differenceY * 0.2f));

			otherM.view = Matrix4x4Multiply(_m, otherM.view);

			otherM.view.vert[3][0] = _x;
			otherM.view.vert[3][1] = _y;
			otherM.view.vert[3][2] = _z;

		}
	}
#pragma endregion

#pragma endregion


#pragma region Buffer Clearing

	devCon->OMSetRenderTargets(1, &backBuffer, pDSV);

	devCon->RSSetViewports(1, &viewport);

	FLOAT f[4];
	//rgba
	f[0] = 0; f[1] = 0; f[2] = 0.1f; f[3] = 0;

	devCon->ClearRenderTargetView(backBuffer, f);
	devCon->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1, 0);
#pragma endregion

#pragma region Grid

	
	//gridWorld.vert[3][2] = 0;
	//gridWorld  = MatrixRotateYDeg(gridWorld, DegToRad(rotate));

	//rotate = 10 * (float)timething.Delta();
	//gridWorld.vert[3][2] = 2;

	timething.Signal();

	//world matrix for grid VS
	otherM.view = Matrix_Inverse(otherM.view);
	D3D11_MAPPED_SUBRESOURCE GridmapRes;
	ZeroMemory(&GridmapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(gridBuff, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &GridmapRes);
	memcpy(GridmapRes.pData, &gridWorld, sizeof(gridWorld));

	devCon->Unmap(gridBuff, NULL);
	
	otherM.view = Matrix_Inverse(otherM.view);

	devCon->VSSetConstantBuffers(0, 1, &gridBuff);

	// TODO: PART 2 STEP 9a
	UINT gridstrides;
	gridstrides = sizeof(Simple_Vertex4);
	UINT gridoffsets = 0;

	devCon->IASetVertexBuffers(0, 1, &GridtriangleBuf, &gridstrides, &gridoffsets);

	devCon->IASetIndexBuffer(gridIndexBuf, DXGI_FORMAT_R32_UINT, gridoffsets);
	// TODO: PART 2 STEP 9b
	devCon->VSSetShader(gridVertexShader, 0, 0);
	devCon->PSSetShader(gridPixelShader, 0, 0);

	// TODO: PART 2 STEP 9c
	devCon->IASetInputLayout(gridLayout);

	// TODO: PART 2 STEP 9d
	devCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	// TODO: PART 2 STEP 10
	devCon->Draw(80, 0);

#pragma endregion

#pragma region Cube 1
	

//	devCon->PSSetShaderResources(0, 1, &shaderRes);
	star1World.vert[3][2] = 0;

	star1World = MatrixRotateYDeg(star1World, DegToRad(rotate));
	rotate = 10 * (float)timething.Delta();
	star1World.vert[3][2] = 2;

	timething.Signal();

	//world matrix for cube VS
	otherM.view = Matrix_Inverse(otherM.view);
	D3D11_MAPPED_SUBRESOURCE mapRes;
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(newBuf[0], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, &star1World, sizeof(star1World));

	devCon->Unmap(newBuf[0], NULL);
	
	//view matrix VS
	ZeroMemory(&mapRes, sizeof(D3D11_MAPPED_SUBRESOURCE));
	tester = devCon->Map(newBuf[1], NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapRes);
	memcpy(mapRes.pData, &otherM, sizeof(otherM));
	devCon->Unmap(newBuf[1], NULL);
	otherM.view = Matrix_Inverse(otherM.view);

	devCon->VSSetConstantBuffers(0, 2, newBuf);

	// TODO: PART 2 STEP 9a
	UINT strides = sizeof(OBJ_VERT);
	UINT offsets = 0;

	devCon->IASetVertexBuffers(0, 1, &CubetriangleBuf, &strides, &offsets);

	devCon->IASetIndexBuffer(indexBuf, DXGI_FORMAT_R32_UINT, offsets);
	// TODO: PART 2 STEP 9b
	devCon->VSSetShader(vertexShader, 0, 0);
	devCon->PSSetShader(pixelShader, 0, 0);

	// TODO: PART 2 STEP 9c
	devCon->IASetInputLayout(layout);

	// TODO: PART 2 STEP 9d
	devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TODO: PART 2 STEP 10
	devCon->DrawIndexed(Cube_index_size, 0, 0);
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
	swap->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
	
	SAFE_RELEASE(GridtriangleBuf);
	SAFE_RELEASE(gridPixelShader);
	SAFE_RELEASE(gridVertexShader);
	SAFE_RELEASE(gridLayout);
	SAFE_RELEASE(gridIndexBuf);
	SAFE_RELEASE(gridBuff);
	SAFE_RELEASE(pTexture2D);
	SAFE_RELEASE(devCon);
	SAFE_RELEASE(dev);
	SAFE_RELEASE(backBuffer);
	SAFE_RELEASE(swap);
	SAFE_RELEASE(layout);
	SAFE_RELEASE(CubetriangleBuf);
	SAFE_RELEASE(indexBuf);
	SAFE_RELEASE(pDSV);
	SAFE_RELEASE(pixelShader);
	SAFE_RELEASE(vertexShader);
	SAFE_RELEASE(newBuf[0]);
	SAFE_RELEASE(newBuf[1]);
	SAFE_RELEASE(pDepthStencil);
	SAFE_RELEASE(rasterStateSolid);
	SAFE_RELEASE(rasterStateWire);

	SAFE_RELEASE(samplerState);
	SAFE_RELEASE(shaderRes);

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