#pragma once
#include "Defines.h"

//structures

struct float2
{
	float x, y;
};


struct float4
{
	float x, y, z, w;
	//float r, g, b, a;
};

struct SIMPLE_VERTEX
{
	XMFLOAT2 point;
	//XMFLOAT4 rgba;

};

struct Vertex4
{
	float vert[4];
	float color;
	//float u, v;
};

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

struct Vertex3
{
	float vert[3];
	unsigned int color;

};

struct Point
{
	int x, y;
};

struct Matrix3x3
{
	float vert[3][3];
};

struct Matrix4x4
{
	float vert[4][4];
	//float u, v;
	//unsigned int color;
};

float DegToRad(float d)
{
	return d* (PI / 180);
}



Vertex4 NDCtoScreenV4(Vertex4 p)
{
	p.vert[0] = p.vert[0] * (RASTER_WIDTH / 2) + (RASTER_WIDTH / 2);
	p.vert[1] = -p.vert[1] * (RASTER_HEIGHT / 2) + (RASTER_HEIGHT / 2);
	//p.vert[2] = 0;
	//p.vert[3] = 0;

	return p;
}

Vertex4 Vert4Matrix4x4Multiply(Vertex4 v, Matrix4x4 m)
{
	Vertex4 _v;
	_v.vert[0] = m.vert[0][0] * v.vert[0] + m.vert[1][0] * v.vert[1] + m.vert[2][0] * v.vert[2] + m.vert[3][0] * v.vert[3];
	_v.vert[1] = m.vert[0][1] * v.vert[0] + m.vert[1][1] * v.vert[1] + m.vert[2][1] * v.vert[2] + m.vert[3][1] * v.vert[3];
	_v.vert[2] = m.vert[0][2] * v.vert[0] + m.vert[1][2] * v.vert[1] + m.vert[2][2] * v.vert[2] + m.vert[3][2] * v.vert[3];
	_v.vert[3] = m.vert[0][3] * v.vert[0] + m.vert[1][3] * v.vert[1] + m.vert[2][3] * v.vert[2] + m.vert[3][3] * v.vert[3];
	_v.color = v.color;
	return _v;
}


Matrix4x4 VertToMatrix(Vertex4 m1)
{
	Matrix4x4 _m;
	_m.vert[0][0] = m1.vert[0];
	_m.vert[0][1] = 0;
	_m.vert[0][2] = 0;
	_m.vert[0][3] = 0;

	_m.vert[1][0] = 0;
	_m.vert[1][1] = m1.vert[1];
	_m.vert[1][2] = 0;
	_m.vert[1][3] = 0;

	_m.vert[2][0] = 0;
	_m.vert[2][1] = 0;
	_m.vert[2][2] = m1.vert[2];
	_m.vert[2][3] = 0;

	_m.vert[3][0] = 0;
	_m.vert[3][1] = 0;
	_m.vert[3][2] = 0;
	_m.vert[3][3] = m1.vert[3];
	//_m.u = m1.u;
	//_m.v = m1.v;
	//_m.color = m1.color;


	return _m;
}

Matrix4x4 Matrix4x4Multiply(Matrix4x4 m1, Matrix4x4 m2)
{
	Matrix4x4 _m;
	_m.vert[0][0] = m1.vert[0][0] * m2.vert[0][0] + m1.vert[0][1] * m2.vert[1][0] + m1.vert[0][2] * m2.vert[2][0] + m1.vert[0][3] * m2.vert[3][0];
	_m.vert[0][1] = m1.vert[0][0] * m2.vert[0][1] + m1.vert[0][1] * m2.vert[1][1] + m1.vert[0][2] * m2.vert[2][1] + m1.vert[0][3] * m2.vert[3][1];
	_m.vert[0][2] = m1.vert[0][0] * m2.vert[0][2] + m1.vert[0][1] * m2.vert[1][2] + m1.vert[0][2] * m2.vert[2][2] + m1.vert[0][3] * m2.vert[3][2];
	_m.vert[0][3] = m1.vert[0][0] * m2.vert[0][3] + m1.vert[0][1] * m2.vert[1][3] + m1.vert[0][2] * m2.vert[2][3] + m1.vert[0][3] * m2.vert[3][3];

	_m.vert[1][0] = m1.vert[1][0] * m2.vert[0][0] + m1.vert[1][1] * m2.vert[1][0] + m1.vert[1][2] * m2.vert[2][0] + m1.vert[1][3] * m2.vert[3][0];
	_m.vert[1][1] = m1.vert[1][0] * m2.vert[0][1] + m1.vert[1][1] * m2.vert[1][1] + m1.vert[1][2] * m2.vert[2][1] + m1.vert[1][3] * m2.vert[3][1];
	_m.vert[1][2] = m1.vert[1][0] * m2.vert[0][2] + m1.vert[1][1] * m2.vert[1][2] + m1.vert[1][2] * m2.vert[2][2] + m1.vert[1][3] * m2.vert[3][2];
	_m.vert[1][3] = m1.vert[1][0] * m2.vert[0][2] + m1.vert[1][1] * m2.vert[1][3] + m1.vert[1][2] * m2.vert[2][3] + m1.vert[1][3] * m2.vert[3][3];

	_m.vert[2][0] = m1.vert[2][0] * m2.vert[0][0] + m1.vert[2][1] * m2.vert[1][0] + m1.vert[2][2] * m2.vert[2][0] + m1.vert[2][3] * m2.vert[3][0];
	_m.vert[2][1] = m1.vert[2][0] * m2.vert[0][1] + m1.vert[2][1] * m2.vert[1][1] + m1.vert[2][2] * m2.vert[2][1] + m1.vert[2][3] * m2.vert[3][1];
	_m.vert[2][2] = m1.vert[2][0] * m2.vert[0][2] + m1.vert[2][1] * m2.vert[1][2] + m1.vert[2][2] * m2.vert[2][2] + m1.vert[2][3] * m2.vert[3][2];
	_m.vert[2][3] = m1.vert[2][0] * m2.vert[0][3] + m1.vert[2][1] * m2.vert[1][3] + m1.vert[2][2] * m2.vert[2][3] + m1.vert[2][3] * m2.vert[3][3];

	_m.vert[3][0] = m1.vert[3][0] * m2.vert[0][0] + m1.vert[3][1] * m2.vert[1][0] + m1.vert[3][2] * m2.vert[2][0] + m1.vert[3][3] * m2.vert[3][0];
	_m.vert[3][1] = m1.vert[3][0] * m2.vert[0][1] + m1.vert[3][1] * m2.vert[1][1] + m1.vert[3][2] * m2.vert[2][1] + m1.vert[3][3] * m2.vert[3][1];
	_m.vert[3][2] = m1.vert[3][0] * m2.vert[0][2] + m1.vert[3][1] * m2.vert[1][2] + m1.vert[3][2] * m2.vert[2][2] + m1.vert[3][3] * m2.vert[3][2];
	_m.vert[3][3] = m1.vert[3][0] * m2.vert[0][3] + m1.vert[3][1] * m2.vert[1][3] + m1.vert[3][2] * m2.vert[2][3] + m1.vert[3][3] * m2.vert[3][3];


	return _m;
}

Vertex4 RotateZDeg(Vertex4 v, float deg)
{
	Matrix4x4 zrot;
	zrot.vert[0][0] = cosf(deg);	zrot.vert[0][1] = sinf(deg); zrot.vert[0][2] = 0; zrot.vert[0][3] = 0;
	zrot.vert[1][0] = -sinf(deg);	zrot.vert[1][1] = cosf(deg); zrot.vert[1][2] = 0; zrot.vert[1][3] = 0;
	zrot.vert[2][0] = 0;			zrot.vert[2][1] = 0;		 zrot.vert[2][2] = 0; zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;			zrot.vert[3][1] = 0;		 zrot.vert[3][2] = 0; zrot.vert[3][3] = 1;

	v = Vert4Matrix4x4Multiply(v, zrot);
	return v;
}

Vertex4 RotateXDeg(Vertex4 v, float deg)
{
	Matrix4x4 zrot;
	zrot.vert[0][0] = 1;	zrot.vert[0][1] = 0;			zrot.vert[0][2] = 0;			zrot.vert[0][3] = 0;
	zrot.vert[1][0] = 0;	zrot.vert[1][1] = cosf(deg);	zrot.vert[1][2] = -sinf(deg);	zrot.vert[1][3] = 0;
	zrot.vert[2][0] = 0;	zrot.vert[2][1] = sinf(deg);	zrot.vert[2][2] = cosf(deg);	zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;	zrot.vert[3][1] = 0;			zrot.vert[3][2] = 0;			zrot.vert[3][3] = 1;

	v = Vert4Matrix4x4Multiply(v, zrot);
	return v;
}

Vertex4 RotateYDeg(Vertex4 v, float deg)
{
	Matrix4x4 zrot;
	
	zrot.vert[0][0] = cosf(deg);	zrot.vert[0][1] = 0;		 zrot.vert[0][2] = sinf(deg);	zrot.vert[0][3] = 0;
	zrot.vert[1][0] = 0;			zrot.vert[1][1] = 1;		 zrot.vert[1][2] = 0;			zrot.vert[1][3] = 0;
	zrot.vert[2][0] = -sinf(deg);	zrot.vert[2][1] = 0;		 zrot.vert[2][2] = cosf(deg);	zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;			zrot.vert[3][1] = 0;		 zrot.vert[3][2] = 0;			zrot.vert[3][3] = 1;

	v = Vert4Matrix4x4Multiply(v, zrot);
	return v;
}

Matrix4x4 Transpose(Matrix4x4 m)
{
	Matrix4x4 mT;
	
	mT.vert[0][0] =m.vert[0][0]; mT.vert[0][1] = m.vert[1][0];		 mT.vert[0][2] = m.vert[2][0];	mT.vert[0][3] = m.vert[3][0];
	mT.vert[1][0] =m.vert[0][1]; mT.vert[1][1] = m.vert[1][1];		 mT.vert[1][2] = m.vert[2][1];	mT.vert[1][3] = m.vert[3][1];
	mT.vert[2][0] =m.vert[0][2]; mT.vert[2][1] = m.vert[1][2];		 mT.vert[2][2] = m.vert[2][2];	mT.vert[2][3] = m.vert[3][2];
	mT.vert[3][0] =m.vert[0][3]; mT.vert[3][1] = m.vert[1][3];		 mT.vert[3][2] = m.vert[2][3];  mT.vert[3][3] = m.vert[3][3];
	
	return mT;
}

Vertex4 Translate(Vertex4 m, Vertex4 t)
{
	

	m.vert[0] += t.vert[0]; 
	m.vert[1] += t.vert[1];		
	m.vert[2] += t.vert[2];	
	

	return m;
}

unsigned int BGRAtoARGB(unsigned int _BGRA)
{
	unsigned int red = 0, green = 0, blue = 0, alpha = 0;
	//separate all colors
	blue = _BGRA & 0xff000000;
	green = _BGRA & 0x00ff0000;
	red = _BGRA & 0x0000ff00;
	alpha = _BGRA & 0x000000ff;
	/*if (alpha == 0)
	{
	return 0;
	}*/
	//clear color
	_BGRA = 0;
	//add them back in in new order using bitshift
	blue = blue >> 24;
	green = green >> 8;
	red = red << 8;
	alpha = alpha << 24;

	_BGRA = (alpha | red | green | blue);
	return _BGRA;
}



Matrix4x4 MatrixTranslate(Matrix4x4 m, Vertex4 t)
{


	m.vert[3][0] += t.vert[0];
	m.vert[3][1] += t.vert[1];
	m.vert[3][2] += t.vert[2];


	return m;
}

int DegreeConverter(unsigned int _x, unsigned int _y, unsigned int _width)
{
	return _x + (_y * _width);
}

float Matrix_Determinant3x3(float e_11, float e_12, float e_13,
	float e_21, float e_22, float e_23,
	float e_31, float e_32, float e_33)
{
	return (e_11 * ((e_22 * e_33) - (e_32 * e_23))) -
		(e_12 * ((e_21 * e_33) - (e_31 * e_23))) +
		(e_13 * ((e_21 * e_32) - (e_31 * e_22)));
}

float Matrix4x4_Determinant(Matrix4x4 m)
{
	// TODO LAB 3: Replace with your implementation.

	float a = m.vert[0][0] * (Matrix_Determinant3x3(m.vert[1][1], m.vert[1][2], m.vert[1][3], m.vert[2][1], m.vert[2][2], m.vert[2][3], m.vert[3][1], m.vert[3][2], m.vert[3][3]));
	float b = m.vert[0][1] * (Matrix_Determinant3x3(m.vert[1][0], m.vert[1][2], m.vert[1][3], m.vert[2][0], m.vert[2][2], m.vert[2][3], m.vert[3][0], m.vert[3][2], m.vert[3][3]));
	float c = m.vert[0][2] * (Matrix_Determinant3x3(m.vert[1][0], m.vert[1][1], m.vert[1][3], m.vert[2][0], m.vert[2][1], m.vert[2][3], m.vert[3][0], m.vert[3][1], m.vert[3][3]));
	float d = m.vert[0][3] * (Matrix_Determinant3x3(m.vert[1][0], m.vert[1][1], m.vert[1][2], m.vert[2][0], m.vert[2][1], m.vert[2][2], m.vert[3][0], m.vert[3][1], m.vert[3][2]));
	float f = a - b + c - d;

	return f;
}

Matrix4x4 Matrix_Inverse(Matrix4x4 m)
{

	float det = Matrix4x4_Determinant(m);
	float g;
	
	if (det == 0.0f)
		return m;
	else g = 1.0f / det;

	// TODO3: Replace with your implementation.

	Matrix4x4 l;

	l.vert[0][0] = g  * (Matrix_Determinant3x3(m.vert[1][1], m.vert[1][2], m.vert[1][3], m.vert[2][1], m.vert[2][2], m.vert[2][3], m.vert[3][1], m.vert[3][2], m.vert[3][3]));
	l.vert[0][1] = -g * (Matrix_Determinant3x3(m.vert[1][0], m.vert[1][2], m.vert[1][3], m.vert[2][0], m.vert[2][2], m.vert[2][3], m.vert[3][0], m.vert[3][2], m.vert[3][3]));
	l.vert[0][2] = g  * (Matrix_Determinant3x3(m.vert[1][0], m.vert[1][1], m.vert[1][3], m.vert[2][0], m.vert[2][1], m.vert[2][3], m.vert[3][0], m.vert[3][1], m.vert[3][3]));
	l.vert[0][3] = -g * (Matrix_Determinant3x3(m.vert[1][0], m.vert[1][1], m.vert[1][2], m.vert[2][0], m.vert[2][1], m.vert[2][2], m.vert[3][0], m.vert[3][1], m.vert[3][2]));
	//l/m.vert[0][0]
	l.vert[1][0] = -g * (Matrix_Determinant3x3(m.vert[0][1], m.vert[0][2], m.vert[0][3], m.vert[2][1], m.vert[2][2], m.vert[2][3], m.vert[3][1], m.vert[3][2], m.vert[3][3]));
	l.vert[1][1] = g  * (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][2], m.vert[0][3], m.vert[2][0], m.vert[2][2], m.vert[2][3], m.vert[3][0], m.vert[3][2], m.vert[3][3]));
	l.vert[1][2] = -g * (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][1], m.vert[0][3], m.vert[2][0], m.vert[2][1], m.vert[2][3], m.vert[3][0], m.vert[3][1], m.vert[3][3]));
	l.vert[1][3] = g  * (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][1], m.vert[0][2], m.vert[2][0], m.vert[2][1], m.vert[2][2], m.vert[3][0], m.vert[3][1], m.vert[3][2]));
	//l/m.vert[0][0]
	l.vert[2][0] = g *  (Matrix_Determinant3x3(m.vert[0][1], m.vert[0][2], m.vert[0][3], m.vert[1][1], m.vert[1][2], m.vert[1][3], m.vert[3][1], m.vert[3][2], m.vert[3][3]));
	l.vert[2][1] = -g * (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][2], m.vert[0][3], m.vert[1][0], m.vert[1][2], m.vert[1][3], m.vert[3][0], m.vert[3][2], m.vert[3][3]));
	l.vert[2][2] = g *  (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][1], m.vert[0][3], m.vert[1][0], m.vert[1][1], m.vert[1][3], m.vert[3][0], m.vert[3][1], m.vert[3][3]));
	l.vert[2][3] = -g * (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][1], m.vert[0][2], m.vert[1][0], m.vert[1][1], m.vert[1][2], m.vert[3][0], m.vert[3][1], m.vert[3][2]));

	l.vert[3][0] = -g * (Matrix_Determinant3x3(m.vert[0][1], m.vert[0][2], m.vert[0][3], m.vert[1][1], m.vert[1][2], m.vert[1][3], m.vert[2][1], m.vert[2][2], m.vert[2][3]));
	l.vert[3][1] = g *  (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][2], m.vert[0][3], m.vert[1][0], m.vert[1][2], m.vert[1][3], m.vert[2][0], m.vert[2][2], m.vert[2][3]));
	l.vert[3][2] = -g * (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][1], m.vert[0][3], m.vert[1][0], m.vert[1][1], m.vert[1][3], m.vert[2][0], m.vert[2][1], m.vert[2][3]));
	l.vert[3][3] = g *  (Matrix_Determinant3x3(m.vert[0][0], m.vert[0][1], m.vert[0][2], m.vert[1][0], m.vert[1][1], m.vert[1][2], m.vert[2][0], m.vert[2][1], m.vert[2][2]));

	l = Transpose(l);

	return l;
}


Matrix4x4 MatrixRotateZDeg(Matrix4x4 v, float deg)
{
	Matrix4x4 zrot;
	zrot.vert[0][0] = cosf(deg);	zrot.vert[0][1] = sinf(deg); zrot.vert[0][2] = 0; zrot.vert[0][3] = 0;
	zrot.vert[1][0] = -sinf(deg);	zrot.vert[1][1] = cosf(deg); zrot.vert[1][2] = 0; zrot.vert[1][3] = 0;
	zrot.vert[2][0] = 0;			zrot.vert[2][1] = 0;		 zrot.vert[2][2] = 0; zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;			zrot.vert[3][1] = 0;		 zrot.vert[3][2] = 0; zrot.vert[3][3] = 1;

	v = Matrix4x4Multiply(v, zrot);
	return v;
}
Matrix4x4 MatrixRotateXDeg(Matrix4x4 v, float deg)
{
	Matrix4x4 zrot;
	zrot.vert[0][0] = 1;	zrot.vert[0][1] = 0;			zrot.vert[0][2] = 0;			zrot.vert[0][3] = 0;
	zrot.vert[1][0] = 0;	zrot.vert[1][1] = cosf(deg);	zrot.vert[1][2] = -sinf(deg);	zrot.vert[1][3] = 0;
	zrot.vert[2][0] = 0;	zrot.vert[2][1] = sinf(deg);	zrot.vert[2][2] = cosf(deg);	zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;	zrot.vert[3][1] = 0;			zrot.vert[3][2] = 0;			zrot.vert[3][3] = 1;

	v = Matrix4x4Multiply(v, zrot);
	return v;
}
Matrix4x4 MatrixRotateYDeg(Matrix4x4 v, float deg)
{
	Matrix4x4 zrot;

	zrot.vert[0][0] = cosf(deg);	zrot.vert[0][1] = 0;		 zrot.vert[0][2] = sinf(deg);	zrot.vert[0][3] = 0;
	zrot.vert[1][0] = 0;			zrot.vert[1][1] = 1;		 zrot.vert[1][2] = 0;			zrot.vert[1][3] = 0;
	zrot.vert[2][0] = -sinf(deg);	zrot.vert[2][1] = 0;		 zrot.vert[2][2] = cosf(deg);	zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;			zrot.vert[3][1] = 0;		 zrot.vert[3][2] = 0;			zrot.vert[3][3] = 1;

	v = Matrix4x4Multiply(v, zrot);
	return v;
}



Matrix4x4 ViewRotateZDeg(Matrix4x4 v, float deg)
{
	Matrix4x4 zrot;
	Vertex4 _v;
	zrot.vert[0][0] = cosf(deg);	zrot.vert[0][1] = sinf(deg); zrot.vert[0][2] = 0; zrot.vert[0][3] = 0;
	zrot.vert[1][0] = -sinf(deg);	zrot.vert[1][1] = cosf(deg); zrot.vert[1][2] = 0; zrot.vert[1][3] = 0;
	zrot.vert[2][0] = 0;			zrot.vert[2][1] = 0;		 zrot.vert[2][2] = 0; zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;			zrot.vert[3][1] = 0;		 zrot.vert[3][2] = 0; zrot.vert[3][3] = 1;

	_v.vert[0] = v.vert[3][0];
	_v.vert[1] = v.vert[3][1];
	_v.vert[2] = v.vert[3][2];
	_v.vert[3] = v.vert[3][3];


	_v = Vert4Matrix4x4Multiply(_v, zrot);

	v.vert[3][0] = _v.vert[0];
	v.vert[3][1] = _v.vert[1];
	v.vert[3][2] = _v.vert[2];
	v.vert[3][3] = _v.vert[3];

	return v;
}
Matrix4x4 ViewRotateXDeg(Matrix4x4 v, float deg)
{
	Matrix4x4 zrot;
	Vertex4 _v;
	zrot.vert[0][0] = 1;	zrot.vert[0][1] = 0;			zrot.vert[0][2] = 0;			zrot.vert[0][3] = 0;
	zrot.vert[1][0] = 0;	zrot.vert[1][1] = cosf(deg);	zrot.vert[1][2] = -sinf(deg);	zrot.vert[1][3] = 0;
	zrot.vert[2][0] = 0;	zrot.vert[2][1] = sinf(deg);	zrot.vert[2][2] = cosf(deg);	zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;	zrot.vert[3][1] = 0;			zrot.vert[3][2] = 0;			zrot.vert[3][3] = 1;

	_v.vert[0] = v.vert[3][0];
	_v.vert[1] = v.vert[3][1];
	_v.vert[2] = v.vert[3][2];
	_v.vert[3] = v.vert[3][3];


	_v = Vert4Matrix4x4Multiply(_v, zrot);

	v.vert[3][0] = _v.vert[0];
	v.vert[3][1] = _v.vert[1];
	v.vert[3][2] = _v.vert[2];
	v.vert[3][3] = _v.vert[3];
	return v;
}
Matrix4x4 ViewRotateYDeg(Matrix4x4 v, float deg)
{
	Matrix4x4 zrot;
	Vertex4 _v;
	zrot.vert[0][0] = cosf(deg);	zrot.vert[0][1] = 0;		 zrot.vert[0][2] = sinf(deg);	zrot.vert[0][3] = 0;
	zrot.vert[1][0] = 0;			zrot.vert[1][1] = 1;		 zrot.vert[1][2] = 0;			zrot.vert[1][3] = 0;
	zrot.vert[2][0] = -sinf(deg);	zrot.vert[2][1] = 0;		 zrot.vert[2][2] = cosf(deg);	zrot.vert[2][3] = 0;
	zrot.vert[3][0] = 0;			zrot.vert[3][1] = 0;		 zrot.vert[3][2] = 0;			zrot.vert[3][3] = 1;

	_v.vert[0] = v.vert[3][0];
	_v.vert[1] = v.vert[3][1];
	_v.vert[2] = v.vert[3][2];
	_v.vert[3] = v.vert[3][3];


	_v = Vert4Matrix4x4Multiply(_v, zrot);

	v.vert[3][0] = _v.vert[0];
	v.vert[3][1] = _v.vert[1];
	v.vert[3][2] = _v.vert[2];
	v.vert[3][3] = _v.vert[3];

	return v;
}


Matrix4x4 SV_WorldMatrix;
void VS_World(Vertex4 &multiplyMe)
{
	multiplyMe = Vert4Matrix4x4Multiply(multiplyMe, SV_WorldMatrix);
}

Matrix4x4 WorldMatrixInit( float x = 0, float y = 0, float z = 2.0f)
{
Matrix4x4 WorldMatrix;
	WorldMatrix.vert[0][0] = 1;		WorldMatrix.vert[0][1] = 0;		WorldMatrix.vert[0][2] = 0;			WorldMatrix.vert[0][3] = 0;
	WorldMatrix.vert[1][0] = 0;		WorldMatrix.vert[1][1] = 1;		WorldMatrix.vert[1][2] = 0;			WorldMatrix.vert[1][3] = 0;
	WorldMatrix.vert[2][0] = 0;		WorldMatrix.vert[2][1] = 0;		WorldMatrix.vert[2][2] = 1;			WorldMatrix.vert[2][3] = 0;
	WorldMatrix.vert[3][0] = x;		WorldMatrix.vert[3][1] = y;		WorldMatrix.vert[3][2] = z;			WorldMatrix.vert[3][3] = 1;
	return WorldMatrix;
}

Matrix4x4 IdentityMatrix()
{
	Matrix4x4 _m;
	_m.vert[0][0] = 1;		_m.vert[0][1] = 0;		_m.vert[0][2] = 0;			_m.vert[0][3] = 0;
	_m.vert[1][0] = 0;		_m.vert[1][1] = 1;		_m.vert[1][2] = 0;			_m.vert[1][3] = 0;
	_m.vert[2][0] = 0;		_m.vert[2][1] = 0;		_m.vert[2][2] = 1;			_m.vert[2][3] = 0;
	_m.vert[3][0] = 0;		_m.vert[3][1] = 0;		_m.vert[3][2] = 0;			_m.vert[3][3] = 1;
	return _m;
}

Matrix4x4 ViewMatrix;
void ViewMatrixInit()
{
	ViewMatrix.vert[0][0] = 1;			ViewMatrix.vert[0][1] = 0.0f;						ViewMatrix.vert[0][2] = 0.0f;					ViewMatrix.vert[0][3] = 0;
	ViewMatrix.vert[1][0] = 0.0f;		ViewMatrix.vert[1][1] = 1;							ViewMatrix.vert[1][2] = 0;						ViewMatrix.vert[1][3] = 0;
	ViewMatrix.vert[2][0] = 0.0f;		ViewMatrix.vert[2][1] = 0;							ViewMatrix.vert[2][2] = 1;						ViewMatrix.vert[2][3] = 0;
	ViewMatrix.vert[3][0] = 0.0f;		ViewMatrix.vert[3][1] = 0.5f;						ViewMatrix.vert[3][2] = 0;						ViewMatrix.vert[3][3] = 1;

}

void ViewMatrixInvert()
{
	Vertex4 tz, vm;

	tz.vert[0] = 0;
	tz.vert[1] = 0;
	tz.vert[2] = 1.0f;
	tz.vert[3] = 1.0f;

	vm.vert[0] = ViewMatrix.vert[0][0];
	vm.vert[1] = ViewMatrix.vert[1][1];
	vm.vert[2] = ViewMatrix.vert[2][2];
	vm.vert[3] = ViewMatrix.vert[3][3];

	vm = Translate(vm, tz);

	Vertex4 pos;
	pos.vert[0] = ViewMatrix.vert[3][0];
	pos.vert[1] = ViewMatrix.vert[3][1];
	pos.vert[2] = ViewMatrix.vert[3][2];

	ViewMatrix = Transpose(ViewMatrix);
	pos = Vert4Matrix4x4Multiply(pos, ViewMatrix);

	ViewMatrix.vert[3][0] = -pos.vert[0];
	ViewMatrix.vert[3][1] = -pos.vert[1];
	ViewMatrix.vert[3][2] = -pos.vert[2];
}


Matrix4x4 ProjPerspectiveMatrix;

void ProjPerspectiveMatrixInit()
{
	float xscale;
	float yscale;

	yscale = 1 / tanf(DegToRad(.5 * FOV));
	xscale = yscale * (RASTER_HEIGHT / RASTER_WIDTH);

	yscale = 1;
	xscale = 1;

	ProjPerspectiveMatrix.vert[0][0] = xscale;	ProjPerspectiveMatrix.vert[0][1] = 0;		ProjPerspectiveMatrix.vert[0][2] = 0;								 ProjPerspectiveMatrix.vert[0][3] = 0;
	ProjPerspectiveMatrix.vert[1][0] = 0;		ProjPerspectiveMatrix.vert[1][1] = yscale;	ProjPerspectiveMatrix.vert[1][2] = 0;								 ProjPerspectiveMatrix.vert[1][3] = 0;
	ProjPerspectiveMatrix.vert[2][0] = 0;		ProjPerspectiveMatrix.vert[2][1] = 0;		ProjPerspectiveMatrix.vert[2][2] = zFar / (zFar - zNear);			 ProjPerspectiveMatrix.vert[2][3] = 1;
	ProjPerspectiveMatrix.vert[3][0] = 0;		ProjPerspectiveMatrix.vert[3][1] = 0;		ProjPerspectiveMatrix.vert[3][2] = -(zFar * zNear) / (zFar - zNear); ProjPerspectiveMatrix.vert[3][3] = 0;

}

struct otherMatrixes
{
	Matrix4x4 view, proj;
};