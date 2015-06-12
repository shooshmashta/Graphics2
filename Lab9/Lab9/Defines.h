#pragma once
// BEGIN PART 1
// TODO: PART 1 STEP 1a
// BEGIN PART 1
// TODO: PART 1 STEP 1a

// TODO: PART 1 STEP 1b
#include <iostream>
#include <Windows.h>
#include <D3D11.h>
#include <DirectXMath.h>
#include <ctime>

#include "Grid_PS.csh"
#include "Grid_VS.csh"
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "Cube.h"
#include "Tron.h"
#include "XTime.h"


#pragma comment (lib, "d3d11.lib")

using namespace std;
using namespace DirectX;


#define FOV 125.0f
#define PI 3.14f
#define RASTER_WIDTH 500
#define RASTER_HEIGHT 500

#define BACKBUFFER_WIDTH	800
#define BACKBUFFER_HEIGHT	800

#define zNear 0.1f
#define zFar 100.0f

#define NUM_PIXELS (RASTER_WIDTH * RASTER_HEIGHT)

#define RED 0xff0000
#define GREEN 0xff00
#define BLUE 0xff
#define YELLOW 0xffff00
#define WHITE 0xffffff
#define BLACK 0x0

#define GRIDSIZE 160
#define SAFE_RELEASE(p) {if(p){p->Release(); p=nullptr;}}