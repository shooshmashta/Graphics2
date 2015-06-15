#pragma once

#include "Lights.h"

struct TriIndexBuffer
{
	unsigned short i0 = 10;
	unsigned short i1 = 0;
	unsigned short i2 = 0;
	//unsigned short i3 = 11;

};
struct Simple_Vertex4
{
	//position
	float x = 0.0f, y = 0.0f, z = 0.0f, w = 1.0f;
	//colors
	float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
	//float u, v;
};

struct Matrix4x4
{
	float vert[4][4];
};

Matrix4x4 ProjPerspectiveMatrix;

void ProjPerspectiveMatrixInit()
{
	float xscale;
	float yscale;

	yscale = 1 / tanf(ToRad(.5 * FOV));
	xscale = yscale * (BACKBUFFER_HEIGHT / BACKBUFFER_WIDTH);

	yscale = 1;
	xscale = 1;

	ProjPerspectiveMatrix.vert[0][0] = xscale;	ProjPerspectiveMatrix.vert[0][1] = 0;		ProjPerspectiveMatrix.vert[0][2] = 0;								 ProjPerspectiveMatrix.vert[0][3] = 0;
	ProjPerspectiveMatrix.vert[1][0] = 0;		ProjPerspectiveMatrix.vert[1][1] = yscale;	ProjPerspectiveMatrix.vert[1][2] = 0;								 ProjPerspectiveMatrix.vert[1][3] = 0;
	ProjPerspectiveMatrix.vert[2][0] = 0;		ProjPerspectiveMatrix.vert[2][1] = 0;		ProjPerspectiveMatrix.vert[2][2] = zFar / (zFar - zNear);			 ProjPerspectiveMatrix.vert[2][3] = 1;
	ProjPerspectiveMatrix.vert[3][0] = 0;		ProjPerspectiveMatrix.vert[3][1] = 0;		ProjPerspectiveMatrix.vert[3][2] = -(zFar * zNear) / (zFar - zNear); ProjPerspectiveMatrix.vert[3][3] = 0;

}


