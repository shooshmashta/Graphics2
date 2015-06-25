//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************
#define MODELCOUNT 6

#include "Math.h"


// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

#define SAFE_RELEASE(p) {if(p){p->Release(); p=nullptr;}}


using namespace std;
void LoadModelOBJThread(const char* path, ObjectModel * model);
void RunThread(ObjectModel * model, ID3D11Device * dev, ID3D11DeviceContext * defCon);
void RunSkyThread(ObjectModel * model, ID3D11Device * dev, ID3D11DeviceContext * defCon, ID3D11DepthStencilView * pDSV);
ObjectModel *RenderObjects(ObjectModel *first, ObjectModel *second, ID3D11Device *dev, ID3D11DeviceContext *devCon, ProjViewMatricies *viewProj);
IDXGISwapChain	*swapGlobal;
//************************************************************
//************ SIMPLE WINDOWS APP CLASS **********************
//************************************************************

class DEMO_APP
{
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	ID3D11RenderTargetView			*backBuffer;
	ID3D11Device					*dev;
	IDXGISwapChain					*swap;
	ID3D11DeviceContext				*devCon;
	ID3D11DeviceContext				*defCon;
	ID3D11CommandList				*comList;
	DXGI_SWAP_CHAIN_DESC			scd;
	D3D11_VIEWPORT					viewport[2];

	thread* threads;
	thread* RunThreads;

	bool fullscreen = false;
	mutex mutex;
	condition_variable condVar;

	HRESULT tester;

	ID3D11RasterizerState * rasterStateSolid;
	ID3D11RasterizerState * rasterStateWire;

	//Simple_Vert star[12];

	//XMMATRIX star1World = XMMatrixIdentity();

	bool wireOn = false;


	//TriIndexBuffer starIndex[20];

	//movement
	LONG lastmouseY = 0;
	LONG lastmouseX = 0;
	float m_pitch, m_yaw;			// orientation euler angles in radians
	XMFLOAT3 m_position;

	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;

	ID3D11DepthStencilView* pDSV;
	ID3D11DepthStencilView* TV_DSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ID3D11Texture2D* pDepthStencil = NULL;


	ID3D11Buffer *newBuf[2];
	XTime timething;

	//lights
#pragma region Lights

	//directional
	Lights light;




#pragma endregion
#pragma region Objects

	//objects
	ObjectModel surface;
	ObjectModel pyramid;
	ObjectModel knight;
	ObjectModel barrel;
	ObjectModel SkyBox;
	ObjectModel Tree;
	//ObjectModel Star;
	//*******************************************************************************************
	//*********************************Render To Texture Objects*********************************
	//*******************************************************************************************

	//ObjectModel TV;
	ID3D11Texture2D* TV_TextureMap;
	ID3D11RenderTargetView* TV_RTVMap;
	ID3D11ShaderResourceView* TV_SRVMap;

	//*******************************************************************************************
	//*******************************************************************************************
	//*******************************************************************************************
	ProjViewMatricies OotherM;
#pragma endregion

	bool rtv = false;



	float rotate;



public:
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
	//star1World = _m + star1World;

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

	OotherM.view = XMMatrixIdentity();

	rotate = 0;

	bool everyother = true;

	
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
	//resizing
	window = CreateWindow(L"DirectXApplication", L"Lab 1a Line Land", WS_OVERLAPPEDWINDOW /*& ~(WS_THICKFRAME | WS_MAXIMIZEBOX)*/,
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
	//********************* END WARNING ************************//


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

	tester = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION,
		&scd, &swap, &dev, NULL, &devCon);


	dev->CreateDeferredContext(0, &defCon);


	swapGlobal = swap;

	////get the address of the back buffer
	ID3D11Texture2D *_BackBuffer = nullptr;
	tester = swap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_BackBuffer));
	//
	//use the back buffer address to create the render target
	tester = dev->CreateRenderTargetView(_BackBuffer, NULL, &backBuffer);
	_BackBuffer->Release();

	//set the render target as the back buffer
	defCon->OMSetRenderTargets(1, &backBuffer, NULL);

	//viewport
	ZeroMemory(&viewport[0], sizeof(D3D11_VIEWPORT));

	viewport[0].MinDepth = 0.0f;
	viewport[0].MaxDepth = 1.0f;
	viewport[0].TopLeftX = 0;
	viewport[0].TopLeftY = 0;
	viewport[0].Width = BACKBUFFER_WIDTH;
	viewport[0].Height = BACKBUFFER_HEIGHT / 2;

	ZeroMemory(&viewport[1], sizeof(D3D11_VIEWPORT));

	viewport[1].MinDepth = 0.0f;
	viewport[1].MaxDepth = 1.0f;
	viewport[1].TopLeftX = 0;
	viewport[1].TopLeftY = BACKBUFFER_HEIGHT / 2;
	viewport[1].Width = BACKBUFFER_WIDTH;
	viewport[1].Height = BACKBUFFER_HEIGHT / 2;

	//set viewport
	defCon->RSSetViewports(2, viewport);


	D3D11_TEXTURE2D_DESC descDepth;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.MiscFlags = NULL;
	descDepth.CPUAccessFlags = NULL;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Width = BACKBUFFER_WIDTH;
	descDepth.Height = BACKBUFFER_HEIGHT;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.Format = DXGI_FORMAT_R32_TYPELESS;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;// | D3D11_BIND_SHADER_RESOURCE;

	tester = dev->CreateTexture2D(&descDepth, NULL, &pDepthStencil);



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

	defCon->OMSetDepthStencilState(pDSState, 1);


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
	rState.FrontCounterClockwise = false;
	dev->CreateRasterizerState(&rState, &rasterStateSolid);

	//ZeroMemory(&rState, sizeof(D3D11_RASTERIZER_DESC));
	//rState.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rState.AntialiasedLineEnable = false;
	rState.MultisampleEnable = false;
	//rState.FrontCounterClockwise = true;
	dev->CreateRasterizerState(&rState, &rasterStateWire);
	defCon->RSSetState(rasterStateSolid);
	pDSState->Release();

#pragma region Objects

	threads = new thread[MODELCOUNT];
	string names[MODELCOUNT];
	int count = 0;
	int *Count = &count;

	threads[0] = thread(LoadModelOBJThread, "SkyBox.obj", &SkyBox);
	threads[1] = thread(LoadModelOBJThread, "sphere.obj", &pyramid);
	threads[2] = thread(LoadModelOBJThread, "Surface.obj", &surface);
	threads[3] = thread(LoadModelOBJThread, "knight.obj", &knight);
	threads[4] = thread(LoadModelOBJThread, "barrel.obj", &barrel);
	threads[5] = thread(LoadModelOBJThread, "Tree.obj", &Tree);
	//threads[6] = thread(LoadModelOBJThread, "Surface.obj", &TV);

	for (int i = 0; i < MODELCOUNT; i++)
	{
		threads[i].join();
	}

	vector<unsigned int> skyIndex;
	int l = SkyBox.vertexIndices.size() - 1;
	for (size_t i = 0; i < SkyBox.vertexIndices.size(); i++)
	{
		skyIndex.push_back(SkyBox.vertexIndices[l]);
		l--;
	}
	SkyBox.vertexIndices = skyIndex;
	SkyBox.SkyInit(L"skyb.dds", dev, defCon, &OotherM);
	Tree.LightsInit(XMFLOAT3(1, -2, 10), L"Tree.dds", L"tree_normal.dds", L"tree_spec.dds", dev, defCon, &OotherM, true);
	pyramid.LightsInit(XMFLOAT3(1, 0, 5), L"energy_seamless.dds", L"energy_seamless_normal.dds", L"energy_seamless_spec.dds", dev, defCon, &OotherM, true);
	surface.LightsInit(XMFLOAT3(0, -2, 4), L"grass_seamless.dds", L"grass_seamless_normal.dds", L"grass_seamless_spec.dds", dev, defCon, &OotherM, false);
	knight.LightsInit(XMFLOAT3(1, -2, 2), L"Knight.dds", L"Knight_normal.dds", L"knight_spec.dds", dev, defCon, &OotherM, true);
	barrel.LightsInit(XMFLOAT3(0, -10, 20), L"barrel.dds", L"barrel_normal.dds", L"barrel_spec.dds", dev, defCon, &OotherM, false);

	//*******************************************************************************************
	//**********************************Setup Render To Texture**********************************
	//*******************************************************************************************

	D3D11_TEXTURE2D_DESC TV_Desc;
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

	ZeroMemory(&TV_Desc, sizeof(D3D11_TEXTURE2D_DESC));

	TV_Desc.Width = BACKBUFFER_WIDTH;
	TV_Desc.Height = BACKBUFFER_HEIGHT;
	TV_Desc.MipLevels = 1;
	TV_Desc.ArraySize = 1;
	TV_Desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	TV_Desc.SampleDesc.Count = 1;
	TV_Desc.SampleDesc.Quality = 0;
	TV_Desc.Usage = D3D11_USAGE_DEFAULT;
	TV_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TV_Desc.CPUAccessFlags = 0;
	TV_Desc.MiscFlags = 0;

	dev->CreateTexture2D(&TV_Desc, NULL, &TV_TextureMap);

	renderTargetViewDesc.Format = TV_Desc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	dev->CreateRenderTargetView(TV_TextureMap, NULL, &TV_RTVMap);

	shaderResourceViewDesc.Format = TV_Desc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;

	dev->CreateShaderResourceView(TV_TextureMap, NULL, &TV_SRVMap);

	//*******************************************************************************************
	//*******************************************************************************************
	//*******************************************************************************************


#pragma endregion

#pragma region Lights

	light.LightsInit(dev, defCon, &OotherM);
	light.SetParameters(defCon, nullptr, &OotherM);


	surface.ObjTexture[0]->Release();
	surface.ObjTexture[0] = nullptr;
	surface.ObjTexture[0] = TV_SRVMap;

#pragma endregion


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

	if ((GetAsyncKeyState('O') & 0x1))
	{
		if (light.spot)
		{
			light.spot = !light.spot;
			light.fourlights.Spot = light.off;
		}
		else
		{
			light.spot = !light.spot;
			light.fourlights.Spot = light.On;
		}
	}

	if ((GetAsyncKeyState('P') & 0x1))
	{
		rtv = !rtv;
	}

	XMFLOAT4 viewMovement;
	if (GetAsyncKeyState('W'))
	{
		viewMovement.x = 0;
		viewMovement.y = 0;
		viewMovement.z = 0.03f;

		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;

	}
	if (GetAsyncKeyState('A'))
	{
		viewMovement.x = -0.03f;
		viewMovement.y = 0;
		viewMovement.z = 0.0f;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('S'))
	{
		viewMovement.x = 0;
		viewMovement.y = 0;
		viewMovement.z = -0.03f;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('D'))
	{

		viewMovement.x = 0.03f;
		viewMovement.y = 0;
		viewMovement.z = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('R') || GetAsyncKeyState(VK_SPACE))
	{

		viewMovement.x = 0;
		viewMovement.y = 0.03f;
		viewMovement.z = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('F') || GetAsyncKeyState(VK_CONTROL) || GetAsyncKeyState('C'))
	{
		viewMovement.x = 0;
		viewMovement.y = -0.03f;
		viewMovement.z = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}


	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		light.fourlights.Directional.dir.x++;
		if (light.fourlights.Directional.dir.x >= 50)
			light.fourlights.Directional.dir.x = 50;

	}
	else if (GetAsyncKeyState(VK_NUMPAD6))
	{
		light.fourlights.Directional.dir.x--;
		if (light.fourlights.Directional.dir.x <= -50)
		{
			light.fourlights.Directional.dir.x = -50;
		}
	}

	if (GetAsyncKeyState(VK_NUMPAD5))
	{
		light.fourlights.Point.position.x -= 1 * 0.1f;
		
	}
	else if (GetAsyncKeyState(VK_NUMPAD8))
	{
		light.fourlights.Point.position.x += 1 * 0.1f;
	}

	if (GetAsyncKeyState(VK_NUMPAD7))
	{
		light.fourlights.Point.direction.x -= 1 * 0.1f;
		if (light.fourlights.Point.direction.x <= -50)
		{
			light.fourlights.Point.direction.x = -50;
		}
	}
	else if (GetAsyncKeyState(VK_NUMPAD9))
	{
		light.fourlights.Point.direction.x += 1 * 0.1f;
		if (light.fourlights.Point.direction.x >= 50)
		{
			light.fourlights.Point.direction.x = 50;
		}
	}


#pragma region Mouse 
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		POINT point;
		if (GetCursorPos(&point))
		{
			long currMouseX = point.x;
			long currMouseY = point.y;

			if (lastmouseX == 0 && lastmouseY == 0)
			{
				lastmouseX = currMouseX;
				lastmouseY = currMouseY;
			}

			int differenceX = currMouseX - lastmouseX;
			int differenceY = currMouseY - lastmouseY;

			lastmouseX = currMouseX;
			lastmouseY = currMouseY;

			float _x = OotherM.view.r[3].m128_f32[0];
			float _y = OotherM.view.r[3].m128_f32[1];
			float _z = OotherM.view.r[3].m128_f32[2];

			OotherM.view.r[3].m128_f32[0] = 0;
			OotherM.view.r[3].m128_f32[1] = 0;
			OotherM.view.r[3].m128_f32[2] = 0;

			XMMATRIX _m = XMMatrixIdentity();

			_m = _m * XMMatrixRotationX(ToRad(differenceY * 0.5f));
			OotherM.view = _m * OotherM.view;

			_m = XMMatrixIdentity();
			_m = _m * XMMatrixRotationY(ToRad(differenceX * 0.5f));
			OotherM.view = OotherM.view * _m;

			OotherM.view.r[3].m128_f32[0] = _x;
			OotherM.view.r[3].m128_f32[1] = _y;
			OotherM.view.r[3].m128_f32[2] = _z;

		}
	}
#pragma endregion



#pragma endregion 
	devCon->OMSetBlendState(0, 0, 0xffffffff);
	defCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	FLOAT f[4]{0, 0, 0, 0};
	ObjectModel* leftover;
	defCon->RSSetViewports(1, &viewport[0]);

	//*******************************************************************************************
	//*********************************Render To Texture Objects*********************************
	//*******************************************************************************************
	defCon->OMSetRenderTargets(1, &TV_RTVMap, pDSV);
	defCon->ClearRenderTargetView(TV_RTVMap, f);
	RunSkyThread(&SkyBox, dev, defCon, pDSV);
	//defCon->ClearRenderTargetView(TV_RTVMap, f);

	//leftover = RenderObjects(&barrel, &surface, dev, defCon, &OotherM);

	leftover = RenderObjects(&barrel, &Tree, dev, defCon, &OotherM);
	leftover = RenderObjects(leftover, &pyramid, dev, defCon, &OotherM);
	leftover = RenderObjects(leftover, &knight, dev, defCon, &OotherM);
	leftover->LightsRun(dev, defCon);
	
	light.SetParameters(defCon, nullptr, &OotherM);
	//surface.textureResource[0]->Release();
	if (rtv)
	{
		surface.ObjTexture[0]->Release();
		surface.ObjTexture[0] = nullptr;
		dev->CreateShaderResourceView(surface.textureResource[0], NULL, &surface.ObjTexture[0]);
	}
	else
	{
		surface.ObjTexture[0]->Release();
		surface.ObjTexture[0] = nullptr;
		dev->CreateShaderResourceView(TV_TextureMap, NULL, &surface.ObjTexture[0]);
	}

	//*******************************************************************************************
	//*******************************************************************************************
	//*******************************************************************************************

#pragma region Buffer Clearing And Skybox

	defCon->OMSetRenderTargets(1, &backBuffer, pDSV);
	defCon->ClearRenderTargetView(backBuffer, f);

#pragma region Sky Box
	RunSkyThread(&SkyBox, dev, defCon, pDSV);
#pragma endregion

#pragma endregion

#pragma region Viewport1

	//on the fly sort

	leftover = RenderObjects(&barrel, &surface, dev, defCon, &OotherM);
	leftover->LightsRun(dev, defCon);

	leftover = RenderObjects(&knight, &Tree, dev, defCon, &OotherM);
	leftover = RenderObjects(leftover, &pyramid, dev, defCon, &OotherM);
	leftover->LightsRun(dev, defCon);
	light.SetParameters(defCon, nullptr, &OotherM);

#pragma endregion
	devCon->OMSetBlendState(0, 0, 0xffffffff);
#pragma region Viewport2
	//defCon->ResolveSubresource(surface.textureResource[0], 0, TV_TextureMap, 0, DXGI_FORMAT_D32_FLOAT);

	defCon->RSSetViewports(1, &viewport[1]);
	RunSkyThread(&SkyBox, dev, defCon, pDSV);

	XMMATRIX view = OotherM.view;
	OotherM.view = XMMatrixTranslation(0, -2, 16);
	//defCon->ResolveSubresource(surface.textureResource[0], 0, TV_TextureMap, 0, DXGI_FORMAT_D32_FLOAT);

	leftover = RenderObjects(&barrel, &surface, dev, defCon, &OotherM);
	leftover->LightsRun(dev, defCon);

	leftover = RenderObjects(&knight, &Tree, dev, defCon, &OotherM);
	leftover = RenderObjects(leftover, &pyramid, dev, defCon, &OotherM);
	leftover->LightsRun(dev, defCon);

	OotherM.view = view;
	light.SetParameters(defCon, nullptr, &OotherM);

#pragma endregion

	defCon->FinishCommandList(true, &comList);
	devCon->ExecuteCommandList(comList, true);
	//defCon->ResolveSubresource(surface.textureResource[0], 0, TV_TextureMap, 0, DXGI_FORMAT_D32_FLOAT);

	comList->Release();
	tester = swap->Present(0, 0);

	return true;
}

//************************************************************
//************ DESTRUCTION ***********************************
//************************************************************

bool DEMO_APP::ShutDown()
{
	swap->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

	SAFE_RELEASE(TV_TextureMap);
	SAFE_RELEASE(TV_RTVMap);
	SAFE_RELEASE(defCon);
	//SAFE_RELEASE(comList);
	SAFE_RELEASE(devCon);
	SAFE_RELEASE(dev);
	SAFE_RELEASE(backBuffer);
	SAFE_RELEASE(swap);
	SAFE_RELEASE(pDSV);
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
	case (WM_SIZE) : {

		int width = LOWORD(lParam);
		int height = HIWORD(lParam);
		
		swapGlobal->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);


	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}




void LoadModelOBJThread(const char* path, ObjectModel * model)
{
	model->loadOBJ(path);
}



void RunThread(ObjectModel * model, ID3D11Device * dev, ID3D11DeviceContext * defCon)
{
	model->LightsRun(dev, defCon);

}


void RunSkyThread(ObjectModel * model, ID3D11Device * dev, ID3D11DeviceContext * defCon, ID3D11DepthStencilView * pDSV)
{
	model->SkyRun(dev, defCon, pDSV);
}


ObjectModel *RenderObjects(ObjectModel *first, ObjectModel *second, ID3D11Device *dev, ID3D11DeviceContext *devCon, ProjViewMatricies *viewProj)
{
	XMVECTOR pos = { 0, 0, 0, 0 };

	pos = XMVector3TransformCoord(pos, first->world);

	float X = XMVectorGetX(pos) - XMVectorGetX(viewProj->view.r[3]);
	float Y = XMVectorGetY(pos) - XMVectorGetY(viewProj->view.r[3]);
	float Z = XMVectorGetZ(pos) - XMVectorGetZ(viewProj->view.r[3]);

	float obj1Dist = X*X + Y*Y + Z*Z;

	pos = XMVectorZero();

	pos = XMVector3TransformCoord(pos, second->world);

	X = XMVectorGetX(pos) - XMVectorGetX(viewProj->view.r[3]);
	Y = XMVectorGetY(pos) - XMVectorGetY(viewProj->view.r[3]);
	Z = XMVectorGetZ(pos) - XMVectorGetZ(viewProj->view.r[3]);

	float obj2Dist = X*X + Y*Y + Z*Z;

	if (obj1Dist > obj2Dist)
	{
		first->LightsRun(dev, devCon);
		return second;
	}
	else
	{
		second->LightsRun(dev, devCon);
		return first;
	}

}


//********************* END WARNING ************************//