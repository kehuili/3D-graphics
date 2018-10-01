/* CS580 Homework 3 */

#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#include	<climits>
#include	<iostream>
#include	<stdio.h>
#define PI (float) 3.14159265358979323846

int GzRender::GzRotXMat(float degree, GzMatrix mat)
{
/* HW 3.1
// Create rotate matrix : rotate along x axis
// Pass back the matrix using mat value
*/
	float rad = degree * PI / 180;
	GzMatrix rotateMatrix = {
		1.0, 0.0,	   0.0,		  0.0,
		0.0, cos(rad), -sin(rad), 0.0,
		0.0, sin(rad), cos(rad),  0.0,
		0.0, 0.0,	   0.0,		  1.0
	};
	copyMatrix(rotateMatrix, mat);
	return GZ_SUCCESS;
}

int GzRender::GzRotYMat(float degree, GzMatrix mat)
{
/* HW 3.2
// Create rotate matrix : rotate along y axis
// Pass back the matrix using mat value
*/
	float rad = degree * PI / 180;
	GzMatrix rotateMatrix = {
		cos(rad),  0.0, sin(rad), 0.0,
		0.0,	   1.0, 0.0,	  0.0,
		-sin(rad), 0.0, cos(rad), 0.0,
		0.0, 	   0.0,	0.0,	  1.0
	};

	copyMatrix(rotateMatrix, mat);
	return GZ_SUCCESS;
}

int GzRender::GzRotZMat(float degree, GzMatrix mat)
{
/* HW 3.3
// Create rotate matrix : rotate along z axis
// Pass back the matrix using mat value
*/
	float rad = degree * PI / 180;
	GzMatrix rotateMatrix = {
		cos(rad), -sin(rad), 0.0, 0.0,
		sin(rad), cos(rad),  0.0, 0.0,
		0.0,	  0.0,		 1.0, 0.0,
		0.0,	  0.0,	     0.0, 1.0
	};
	copyMatrix(rotateMatrix, mat);
	return GZ_SUCCESS;
}

int GzRender::GzTrxMat(GzCoord translate, GzMatrix mat)
{
/* HW 3.4
// Create translation matrix
// Pass back the matrix using mat value
*/
	GzMatrix transMatrix = {
		1.0, 0.0, 0.0, translate[X],
		0.0, 1.0, 0.0, translate[Y],
		0.0, 0.0, 1.0, translate[Z],
		0.0, 0.0, 0.0, 1.0
	};
	copyMatrix(transMatrix, mat);
	return GZ_SUCCESS;
}


int GzRender::GzScaleMat(GzCoord scale, GzMatrix mat)
{
/* HW 3.5
// Create scaling matrix
// Pass back the matrix using mat value
*/
	GzMatrix scaleMatrix = {
		scale[X], 0.0,		0.0,	  0.0,
		0.0,	  scale[Y], 0.0,	  0.0,
		0.0,	  0.0,		scale[Z], 0.0,
		0.0,	  0.0,		0.0,	  1.0
	};
	copyMatrix(scaleMatrix, mat);
	return GZ_SUCCESS;
}


GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = xRes;
	yres = yRes;
	framebuffer = (char*)malloc(3 * sizeof(char) * xRes * yRes);
	pixelbuffer = (GzPixel*)malloc(sizeof(GzPixel) * xRes * yRes);

/* HW 3.6
- setup Xsp and anything only done once 
- init default camera 
*/ 
	// setup Xsp
	GzMatrix temp = {
		xres / 2.0, 0.0,		 0.0,	  xres / 2.0,
		0.0,		-yres / 2,   0.0,	  yres / 2.0,
		0.0,	    0.0,		 INT_MAX, 0.0,
		0.0,	    0.0,		 0.0,	  1.0
	};
	copyMatrix(temp, Xsp);
	matlevel = 0;
	// init default camera
	m_camera.position[X] = DEFAULT_IM_X;
	m_camera.position[Y] = DEFAULT_IM_Y;
	m_camera.position[Z] = DEFAULT_IM_Z;

	m_camera.lookat[X] = 0.0;
	m_camera.lookat[Y] = 0.0;
	m_camera.lookat[Z] = 0.0;

	m_camera.worldup[X] = 0.0;
	m_camera.worldup[Y] = 1.0;
	m_camera.worldup[Z] = 0.0;

	m_camera.FOV = DEFAULT_FOV;
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	free(framebuffer);
	free(pixelbuffer);
}

int GzRender::GzDefault()
{
/* HW1.3 set pixel buffer to some default values - start a new frame */
	for (int i = 0; i < yres; i++) {
		for (int j = 0; j < xres; j++) {
			pixelbuffer[j * xres + i].red = 4095;
			pixelbuffer[j * xres + i].green = 4095;
			pixelbuffer[j * xres + i].blue = 4095;
			pixelbuffer[j * xres + i].alpha = 4095;
			pixelbuffer[j * xres + i].z = INT_MAX;
		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzBeginRender()
{
/* HW 3.7 
- setup for start of each frame - init frame buffer color,alpha,z
- compute Xiw and projection xform Xpi from camera definition 
- init Ximage - put Xsp at base of stack, push on Xpi and Xiw 
- now stack contains Xsw and app can push model Xforms when needed 
*/ 
	// setup frame
	//GzFlushDisplay2FrameBuffer()
	for (int j = 0; j < yres; j++) {
		for (int i = 0; i < xres; i++) {
			framebuffer[3 * (j * xres + i)] = 4095;
			framebuffer[3 * (j * xres + i) + 1] = 4095;
			framebuffer[3 * (j * xres + i) + 2] = 4095;
		}
	}
	// compute Xiw
	GzCoord axisX, axisY, axisZ, up, c;
	float normX, normY, normZ, dotProd;
	float param = tan(m_camera.FOV * PI / 180 / 2);
	// norm Z axis
	axisZ[X] = m_camera.lookat[X] - m_camera.position[X];
	axisZ[Y] = m_camera.lookat[Y] - m_camera.position[Y];
	axisZ[Z] = m_camera.lookat[Z] - m_camera.position[Z];
	normZ = sqrt(axisZ[X] * axisZ[X] + axisZ[Y] * axisZ[Y] + axisZ[Z] * axisZ[Z]);
	axisZ[X] /= normZ;
	axisZ[Y] /= normZ;
	axisZ[Z] /= normZ;
	// norm Y axis
	up[X] = m_camera.worldup[X];
	up[Y] = m_camera.worldup[Y];
	up[Z] = m_camera.worldup[Z];
	dotProd = dot(up, axisZ);
	axisY[X] = up[X] - dotProd * axisZ[X];
	axisY[Y] = up[Y] - dotProd * axisZ[Y];
	axisY[Z] = up[Z] - dotProd * axisZ[Z];
	normY = sqrt(axisY[X] * axisY[X] + axisY[Y] * axisY[Y] + axisY[Z] * axisY[Z]);
	axisY[X] /= normY;
	axisY[Y] /= normY;
	axisY[Z] /= normY;
	// X axis
	axisX[X] = axisY[Y] * axisZ[Z] - axisY[Z] * axisZ[Y];
	axisX[Y] = axisY[Z] * axisZ[X] - axisY[X] * axisZ[Z];
	axisX[Z] = axisY[X] * axisZ[Y] - axisY[Y] * axisZ[X];
	// Xiw
	c[X] = m_camera.position[X];
	c[Y] = m_camera.position[Y];
	c[Z] = m_camera.position[Z];

	GzMatrix Xiw = {
		axisX[X], axisX[Y], axisX[Z], -dot(axisX, c),
		axisY[X], axisY[Y], axisY[Z], -dot(axisY, c),
		axisZ[X], axisZ[Y], axisZ[Z], -dot(axisZ, c),
		0.0,	  0.0,		0.0,	  1.0
	};
	copyMatrix(Xiw, m_camera.Xiw);

	// Xpi
	GzMatrix Xpi = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, param, 0.0,
		0.0, 0.0, param, 1.0
	};
	copyMatrix(Xpi, m_camera.Xpi);

	// init Ximage
	GzPushMatrix(Xsp);
	GzPushMatrix(m_camera.Xpi);
	GzPushMatrix(m_camera.Xiw);
	return GZ_SUCCESS;
}

int GzRender::GzPutCamera(GzCamera camera)
{
/* HW 3.8 
/*- overwrite renderer camera structure with new camera definition
*/
	m_camera.position[X] = camera.position[X];
	m_camera.position[Y] = camera.position[Y];
	m_camera.position[Z] = camera.position[Z];

	m_camera.lookat[X] = camera.lookat[X];
	m_camera.lookat[Y] = camera.lookat[Y];
	m_camera.lookat[Z] = camera.lookat[Z];

	m_camera.worldup[X] = camera.worldup[X];
	m_camera.worldup[Y] = camera.worldup[Y];
	m_camera.worldup[Z] = camera.worldup[Z];

	m_camera.FOV = camera.FOV;
	return GZ_SUCCESS;	
}

int GzRender::GzPushMatrix(GzMatrix	matrix)
{
/* HW 3.9 
- push a matrix onto the Ximage stack
- check for stack overflow
*/
	if (matlevel >= MATLEVELS) {
		return GZ_FAILURE;
	}
	if (matlevel == 0) {
		copyMatrix(matrix, Ximage[++matlevel]);
		return GZ_SUCCESS;
	}
	GzMatrix res;
	multiply(Ximage[matlevel], matrix, res);
	copyMatrix(res, Ximage[++matlevel]);

	return GZ_SUCCESS;
}

int GzRender::GzPopMatrix()
{
/* HW 3.10
- pop a matrix off the Ximage stack
- check for stack underflow
*/
	if (matlevel == 0) {
		return GZ_FAILURE;
	}
	matlevel--;
	return GZ_SUCCESS;
}

int GzRender::GzPut(int i, int j, GzIntensity r, GzIntensity g, GzIntensity b, GzIntensity a, GzDepth z)
{
/* HW1.4 write pixel values into the buffer */
	// frame bound 
	i = inBound(xres, 0, i);
	j = inBound(yres, 0, j);
	r = inBound(MAXCOLOR, 0, r);
	g = inBound(MAXCOLOR, 0, g);
	b = inBound(MAXCOLOR, 0, b);

	pixelbuffer[j * xres + i].red = r;
	pixelbuffer[j * xres + i].green = g;
	pixelbuffer[j * xres + i].blue = b;
	pixelbuffer[j * xres + i].alpha = a;
	pixelbuffer[j * xres + i].z = z;
	return GZ_SUCCESS;
}


int GzRender::GzGet(int i, int j, GzIntensity *r, GzIntensity *g, GzIntensity *b, GzIntensity *a, GzDepth *z)
{
/* HW1.5 retrieve a pixel information from the pixel buffer */
	r = &pixelbuffer[j * xres + i].red;
	g = &pixelbuffer[j * xres + i].green;
	b = &pixelbuffer[j * xres + i].blue;
	a = &pixelbuffer[j * xres + i].alpha;
	z = &pixelbuffer[j * xres + i].z;
	return GZ_SUCCESS;
}


int GzRender::GzFlushDisplay2File(FILE* outfile)
{
/* HW1.6 write image to ppm file -- "P6 %d %d 255\r" */
	fprintf(outfile, "P6 %d %d 255\n", xres, yres);
	for (int j = 0; j < yres; j++) {
		for (int i = 0; i < xres; i++) {
			uint8_t r = pixelbuffer[j * xres + i].red >> 4;
			uint8_t g = pixelbuffer[j * xres + i].green >> 4;
			uint8_t b = pixelbuffer[j * xres + i].blue >> 4;
			uint8_t buffer[3] = { r, g, b };
			fwrite(buffer, 1, 3, outfile);
		}
	}

	return GZ_SUCCESS;
}

int GzRender::GzFlushDisplay2FrameBuffer()
{
/* HW1.7 write pixels to framebuffer: 
	- put the pixels into the frame buffer
	- CAUTION: when storing the pixels into the frame buffer, the order is blue, green, and red 
	- NOT red, green, and blue !!!
*/
	for (int j = 0; j < yres; j++) {
		for (int i = 0; i < xres; i++) {
			GzIntensity r = pixelbuffer[j * xres + i].red >> 4;
			GzIntensity g = pixelbuffer[j * xres + i].green >> 4;
			GzIntensity b = pixelbuffer[j * xres + i].blue >> 4;
			
			framebuffer[3 * (j * xres + i)] = b;
			framebuffer[3 * (j * xres + i) + 1] = g;
			framebuffer[3 * (j * xres + i) + 2] = r;
		}
	}
	return GZ_SUCCESS;
}
int GzRender::inBound(int upper, int lower, int val) {
	if (val < lower) {
		return lower;
	}
	else if (val >= upper) {
		return upper - 1;
	}
	else {
		return val;
	}
}

/***********************************************/
/* HW2 methods: implement from here */

int GzRender::GzPutAttribute(int numAttributes, GzToken	*nameList, GzPointer *valueList)
{
	/* HW 2.1
	-- Set renderer attribute states (e.g.: GZ_RGB_COLOR default color)
	-- In later homeworks set shaders, interpolaters, texture maps, and lights
	*/
	for (int i = 0; i < numAttributes; i++) {
		switch (nameList[i])
		{
		case GZ_RGB_COLOR:
		{
			float* cr = (float*)valueList[i];
			flatcolor[0] = cr[0];
			flatcolor[1] = cr[1];
			flatcolor[2] = cr[2];
			break;
		}
		case GZ_SHADER:
			break;
		default:
			break;
		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int	numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
	/* HW 2.2
	-- Pass in a triangle description with tokens and values corresponding to
	GZ_NULL_TOKEN:		do nothing - no values
	GZ_POSITION:		3 vert positions in model space
	-- Invoke the rastrizer/scanline framework
	-- Return error code
	*/
	for (int i = 0; i < numParts; i++) {
		switch (nameList[i])
		{
		case GZ_NULL_TOKEN:
			break;
		case GZ_POSITION:
		{
			GzCoord* coord = (GzCoord*)valueList[i];
			// apply stack to vertex
			for (int i = 0; i < 3; i++) {
				GzCoord res;
				if (vectorMultiply(Ximage[matlevel], coord[i], res) == GZ_FAILURE) {
					return GZ_FAILURE;
				}
				coord[i][X] = res[X];
				coord[i][Y] = res[Y];
				coord[i][Z] = res[Z];
			}
			// vertex sorting
			float y0 = coord[0][1], y1 = coord[1][1], y2 = coord[2][1];
			int *sort = sortVert(y0, y1, y2);

			float *params = computeExpression(coord[sort[2]], coord[sort[0]]);
			float x = (-params[1] * coord[sort[1]][1] - params[2]) / params[0];

			if (coord[sort[1]][0] < x) {
				float temp = sort[1];
				sort[1] = sort[2];
				sort[2] = temp;
			}

			// compute plane
			GzCoord e0 = { coord[0][0] - coord[1][0],
				coord[0][1] - coord[1][1],
				coord[0][2] - coord[1][2] };
			GzCoord e1 = { coord[0][0] - coord[2][0],
				coord[0][1] - coord[2][1],
				coord[0][2] - coord[2][2] };
			float *pL = computePlane(e0, e1, coord[0]);

			// compute A, B, C for 3 edges
			float *p0 = computeExpression(coord[sort[0]], coord[sort[1]]);
			float *p1 = computeExpression(coord[sort[1]], coord[sort[2]]);
			float *p2 = computeExpression(coord[sort[2]], coord[sort[0]]);

			int minX = min3(coord[0][0], coord[1][0], coord[2][0]);
			int minY = min3(coord[0][1], coord[1][1], coord[2][1]);
			int maxX = max3(coord[0][0], coord[1][0], coord[2][0]);
			int maxY = max3(coord[0][1], coord[1][1], coord[2][1]);

			minX = inBound(xres, 0, minX);
			minY = inBound(yres, 0, minY);
			maxX = inBound(xres, 0, maxX);
			maxY = inBound(yres, 0, maxY);

			for (int j = minY; j <= maxY; j++) {
				for (int i = minX; i <= maxX; i++) {
					if (LEE(p0, i, j) <= 0
						&& LEE(p1, i, j) <= 0
						&& LEE(p2, i, j) < 0) {
						int zPix = (-pL[3] - pL[0] * i - pL[1] * j) / pL[2];
						// ignore z behind view plane
						// ignore larger z
						if (zPix >= 0 && zPix < pixelbuffer[j * xres + i].z) {
							GzIntensity r = inBound(MAXCOLOR, 0, ctoi(flatcolor[0]));
							GzIntensity g = inBound(MAXCOLOR, 0, ctoi(flatcolor[1]));
							GzIntensity b = inBound(MAXCOLOR, 0, ctoi(flatcolor[2]));
							pixelbuffer[j * xres + i].red = r;
							pixelbuffer[j * xres + i].green = g;
							pixelbuffer[j * xres + i].blue = b;
							pixelbuffer[j * xres + i].z = zPix;
						}
					}
				}
			}
			break;
		}
		case GZ_NORMAL:
			break;
		case GZ_TEXTURE_INDEX:
			break;
		default:
			break;
		}
	}
	return GZ_SUCCESS;
}

int* GzRender::sortVert(float y0, float y1, float y2) {
	int *result = new int[3];
	int min = 0, mid = 1, max = 2;
	if (y0 <= y1 && y0 <= y2) {
		if (y1 > y2) {
			mid = 2;
			max = 1;
		}
	}
	else if (y1 <= y0 && y1 <= y2) {
		min = 1;
		mid = 0;
		if (y0 > y2) {
			mid = 2;
			max = 0;
		}
	}
	else {
		min = 2;
		max = 0;
		if (y1 > y0) {
			mid = 0;
			max = 1;
		}
	}
	result[0] = min;
	result[1] = mid;
	result[2] = max;
	return result;
}

float* GzRender::computeExpression(GzCoord tail, GzCoord head) {
	float *result = new float[3];
	float dy = head[1] - tail[1];
	float dx = head[0] - tail[0];
	float x = tail[0];
	float y = tail[1];
	// Ax + By + C = 0
	result[0] = dy;// A
	result[1] = -dx;// B
	result[2] = dx * y - dy * x;// C
	return result;
}

int GzRender::max3(float a, float b, float c) {
	int max = a > b ? a : b;
	max = c > max ? c : max;
	return max;
}

int GzRender::min3(float a, float b, float c) {
	int min = a < b ? a : b;
	min = c < min ? c : min;
	return min;
}

float GzRender::LEE(float *params, int x, int y) {
	return params[0] * x + params[1] * y + params[2];
}

float* GzRender::computePlane(GzCoord v0, GzCoord v1, GzCoord vert) {
	float* result = new float[4];
	float a = v0[1] * v1[2] - v0[2] * v1[1];
	float b = v0[2] * v1[0] - v0[0] * v1[2];
	float c = v0[0] * v1[1] - v0[1] * v1[0];
	float d = -a * vert[0] - b * vert[1] - c * vert[2];
	result[0] = a;
	result[1] = b;
	result[2] = c;
	result[3] = d;
	return result;
}

void GzRender::multiply(GzMatrix m0, GzMatrix m1, GzMatrix res) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			res[i][j] = 0.0;
			for (int k = 0; k < 4; k++) {
				res[i][j] += m0[i][k] * m1[k][j];
			}
		}
	}
	return;
}

void GzRender::copyMatrix(GzMatrix m0, GzMatrix m1) {
	// copy matrix m0 to m1
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			m1[i][j] = m0[i][j];
		}
	}
	return;
}

float GzRender::dot(GzCoord v0, GzCoord v1) {
	return v0[X] * v1[X] + v0[Y] * v1[Y] + v0[Z] * v1[Z];
}

int GzRender::vectorMultiply(GzMatrix m, GzCoord c, GzCoord res) {
	float w = 0.0;
	// X, Y, Z
	for (int i = 0; i < 3; i++) {
		res[i] = 0.0;
		for (int j = 0; j < 3; j++) {
			res[i] += m[i][j] * c[j];
		}
		// 3D -> 4D, w = 1
		res[i] += m[i][3];
	}
	if (res[Z] < 0) {
		return GZ_FAILURE;
	}
	// w
	for (int j = 0; j < 3; j++) {
		w += m[3][j] * c[j];
	}
	w += m[3][3];
	// 4D to 3D
	res[X] /= w;
	res[Y] /= w;
	res[Z] /= w;

	return GZ_SUCCESS;
}