#pragma once

#include <iostream>
#include <Windows.h>
#include <D3D11.h>
#include <DirectXMath.h>
#include <ctime>

#include <vector>
#include <sstream>
#include <dwrite.h>
#include <dinput.h>
#include <vector>
#include <fstream>
#include <istream>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "DDSTextureLoader.h"
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "Sky_PS.csh"
#include "Sky_VS.csh"

#include "Combined_PS.csh"
#include "Combined_VS.csh"

#include "XTime.h"

#pragma comment (lib, "d3d11.lib")

using namespace std;
using namespace DirectX;



#define FOV 125.0f
#define PI 3.14159265f


#define BACKBUFFER_WIDTH	800
#define BACKBUFFER_HEIGHT	800

#define zNear 0.1f
#define zFar 100.0f

#define NUM_LIGHTS 4
#define NUM_PIXELS (RASTER_WIDTH * RASTER_HEIGHT)

#define RED 0xff0000
#define GREEN 0xff00
#define BLUE 0xff
#define YELLOW 0xffff00
#define WHITE 0xffffff
#define BLACK 0x0

#define GRIDSIZE 160
#define SAFE_RELEASE(p) {if(p){p->Release(); p=nullptr;}}

#define ToDegree(rad) ((rad) * (180.0f / PI))
#define ToRad(deg) ((deg) * ( PI / 180.0f))

//movement
#define ROTATION_GAIN 0.004f