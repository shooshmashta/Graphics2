//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************


#include "Math.h"


// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

#define SAFE_RELEASE(p) {if(p){p->Release(); p=nullptr;}}


using namespace std;

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

	Simple_Vert star[12];

	XMMATRIX star1World = XMMatrixIdentity();

	bool wireOn = false;
	

	TriIndexBuffer starIndex[20];

	//movement
	LONG lastmouseY = 0;
	LONG lastmouseX = 0;
	float m_pitch, m_yaw;			// orientation euler angles in radians
	XMFLOAT3 m_position;

	ID3D11PixelShader *pixelShader;
	ID3D11VertexShader *vertexShader;

	ID3D11DepthStencilView* pDSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ID3D11Texture2D* pDepthStencil = NULL;
	

	ID3D11Buffer *newBuf[2];
	XTime timething;

	ObjectModel pyramid;
	ObjectModel starTester;
	ProjViewMatricies OotherM;

	struct SEND_TO_VRAM
	{
		XMFLOAT4 point;

	};
	float rotate;

	SEND_TO_VRAM toShader;
	



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

	bool everyother = true;

	for (int i = 0; i < 10; i++)
	{

		if (everyother)
		{
			everyother = false;
			star[i].m_vect.x = cosf(ToRad(36.0f * (float)(i)));
			star[i].m_vect.y = sinf(ToRad(36.0f * (float)(i)));
		}
		else
		{
			star[i].m_vect.x = cosf(ToRad(36.0f * (float)i)) / 2;
			star[i].m_vect.y = sinf(ToRad(36.0f * (float)i)) / 2;
			everyother = true;
		}

		star[i].m_color.x = 0.5f;
		star[i].m_color.y = 0.25f;

	}

	star[10].m_vect.z = 0.3f;
	star[11].m_vect.z = -0.3f;

	star[10].m_color.x = 1;
	star[10].m_color.y = 1;
	star[11].m_color.x = 1;
	star[11].m_color.y = 1;


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

	////get the address of the back buffer
	ID3D11Texture2D *_BackBuffer = nullptr;
	tester = swap->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&_BackBuffer));
	//
	//use the back buffer address to create the render target
	tester = dev->CreateRenderTargetView(_BackBuffer, NULL, &backBuffer);
	_BackBuffer->Release();

	//set the render target as the back buffer
	devCon->OMSetRenderTargets(1, &backBuffer, NULL);

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
	rState.FrontCounterClockwise = false;
	dev->CreateRasterizerState(&rState, &rasterStateSolid);

	//ZeroMemory(&rState, sizeof(D3D11_RASTERIZER_DESC));
	//rState.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
	rState.AntialiasedLineEnable = false;
	rState.MultisampleEnable = false;
	rState.FrontCounterClockwise = true;
	dev->CreateRasterizerState(&rState, &rasterStateWire);
	devCon->RSSetState(rasterStateSolid);
	pDSState->Release();
	


	pyramid.loadOBJ("pyramid1.obj");
	pyramid.Init(XMFLOAT3(1, 1, 1), dev, devCon, &OotherM, true);

	Simple_Vert _v;
	StrideStruct _s;

	for (size_t i = 0; i < 12; i++)
	{
		_v = star[i];

		_s.m_vect = star[i].m_vect;

		starTester.v_vertices.push_back(_v);
		starTester.m_stride.push_back(_s);
	}
	for (size_t i = 0; i < 20; i++)
	{
		starTester.vertexIndices.push_back(starIndex[i].i0);
		starTester.vertexIndices.push_back(starIndex[i].i1);
		starTester.vertexIndices.push_back(starIndex[i].i2);
	}
	//do not set to true
	starTester.Init(XMFLOAT3(-1, 0, 1), dev, devCon, &OotherM, false);
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
	
	XMFLOAT4 viewMovement;
	if (GetAsyncKeyState('W'))
	{
		viewMovement.x= 0;
		viewMovement.y = 0;
		viewMovement.z = 0.01f;

		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;


	}
	if (GetAsyncKeyState('A'))
	{
		viewMovement.x = -0.01f;
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
		viewMovement.z = -0.01f;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('D'))
	{

		viewMovement.x = 0.01f;
		viewMovement.y = 0;
		viewMovement.z = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('R'))
	{

		viewMovement.x = 0;
		viewMovement.y = 0.01f;
		viewMovement.z = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}
	if (GetAsyncKeyState('F'))
	{
		viewMovement.x = 0;
		viewMovement.y = -0.01f;
		viewMovement.z = 0;
		XMMATRIX _m = XMMatrixIdentity();
		_m = XMMatrixTranslation(viewMovement.x, viewMovement.y, viewMovement.z);
		OotherM.view = _m * OotherM.view;
	}


	if (GetAsyncKeyState(VK_NUMPAD4))
	{
		XMMATRIX _m = XMMatrixIdentity();
		_m = _m * XMMatrixRotationY(ToRad(-0.1f));


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
		_m = _m * XMMatrixRotationY(ToRad(0.1f));

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
		OotherM.view = OotherM.view * XMMatrixRotationX(ToRad(0.1f));
	}
	else if (GetAsyncKeyState(VK_NUMPAD8))
	{
		OotherM.view = OotherM.view * XMMatrixRotationX(ToRad(-0.1f));
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

			XMMATRIX _m = XMMatrixIdentity();

			_m = _m * XMMatrixRotationX(ToRad(differenceY * 0.2f));
			float _x = OotherM.view.r[3].m128_f32[0];
			float _y = OotherM.view.r[3].m128_f32[1];
			float _z = OotherM.view.r[3].m128_f32[2];
			OotherM.view.r[3].m128_f32[0] = 0;
			OotherM.view.r[3].m128_f32[1] = 0;
			OotherM.view.r[3].m128_f32[2] = 0;

			
			_m = _m * XMMatrixRotationY(ToRad(differenceX * 0.2f));

			OotherM.view = _m * OotherM.view;

			OotherM.view.r[3].m128_f32[0] = _x;
			OotherM.view.r[3].m128_f32[1] = _y;
			OotherM.view.r[3].m128_f32[2] = _z;


			m_pitch -= differenceY;		// mouse y increases down, but pitch increases up
			m_yaw -= differenceX;

			m_pitch = (float)__max(-PI / 2.0f, m_pitch);
			m_pitch = (float)__min(+PI / 2.0f, m_pitch);

		}
	}
#pragma endregion



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



#pragma region Star 2

	pyramid.Run(dev, devCon);
	starTester.Run(dev, devCon);
	starTester.RunNewPos(XMFLOAT3(0, 2, 5), dev, devCon);
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
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//