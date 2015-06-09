#pragma once
// BEGIN PART 1
// TODO: PART 1 STEP 1a
// BEGIN PART 1
// TODO: PART 1 STEP 1a
#include <D3D11.h>
#pragma comment (lib, "d3d11.lib")

// TODO: PART 1 STEP 1b
#include <DirectXMath.h>
using namespace DirectX;
#include <iostream>
#include <Windows.h>


#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "Cube.h"
#include "Tron.h"
//#include <time.h>

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

