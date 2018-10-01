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
	framebuffer = (char*) malloc (3 * sizeof(char) * xRes * yRes);
	pixelbuffer = (GzPixel*) malloc (sizeof(GzPixel) * xRes * yRes);

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
/* HW4.1 init ks, kd, ka*/
	GzColor ks = DEFAULT_SPECULAR;
	GzColor kd = DEFAULT_DIFFUSE;
	GzColor ka = DEFAULT_AMBIENT;
	copyColor(ks, Ks);
	copyColor(kd, Kd);
	copyColor(ka, Ka);

	numlights = 0;
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
	GzMatrix I = {
		1.0, 0.0, 0.0, 0.0,
		0.0, 1.0, 0.0, 0.0,
		0.0, 0.0, 1.0, 0.0,
		0.0, 0.0, 0.0, 1.0
	};
	if (matlevel >= MATLEVELS) {
		return GZ_FAILURE;
	}
	if (matlevel == 0) { // Xsp
		copyMatrix(matrix, Ximage[++matlevel]);
		copyMatrix(I, Xnorm[matlevel]);
		return GZ_SUCCESS;
	}
	// Ximage
	GzMatrix res;
	multiply(Ximage[matlevel], matrix, res);
	copyMatrix(res, Ximage[matlevel + 1]);
	//Xnorm
	if (matlevel == 1) { // Xpi
		copyMatrix(I, Xnorm[matlevel + 1]);
	} else { // Xiw, Xwm
		GzMatrix res;
		// remove translation
		matrix[0][3] = 0.0;
		matrix[1][3] = 0.0;
		matrix[2][3] = 0.0;
		// unitary rotation
		float k = sqrt((pow(matrix[0][0], 2) + pow(matrix[0][1], 2) + pow(matrix[0][2], 2)));
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				matrix[i][j] /= k;
			}
		}
		multiply(Xnorm[matlevel], matrix, res);
		copyMatrix(res, Xnorm[matlevel + 1]);
	}
	matlevel++;
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
			//interp_mode = GZ_FLAT;
			break;
		}
		/* HW 4.2 set light, shaders, interpolaters*/
		case GZ_INTERPOLATE: // interpolate mode
		{
			interp_mode = *(int*)valueList[i];
			break;
		}
		case GZ_DIRECTIONAL_LIGHT: 
		{
			GzLight light = *(GzLight*)valueList[i];
			lights[numlights++] = light;
			break;
		}
		case GZ_AMBIENT_LIGHT: 
		{
			GzLight light = *(GzLight*)valueList[i];
			ambientlight = light;
			break;
		}
		case GZ_SPECULAR_COEFFICIENT:
		{
			float* ks = (float*)valueList[i];
			copyColor(ks, Ks);
			break;
		}
		case GZ_DIFFUSE_COEFFICIENT:
		{
			float* kd = (float*)valueList[i];
			copyColor(kd, Kd);
			break;
		}
		case GZ_AMBIENT_COEFFICIENT: 
		{
			float* ka = (float*)valueList[i];
			copyColor(ka, Ka);
			break;
		}
		case GZ_DISTRIBUTION_COEFFICIENT:
		{
			spec = *(float*)valueList[i];
			break;
		}
		default:
			break;
		}
	}
	return GZ_SUCCESS;
}

int GzRender::GzPutTriangle(int numParts, GzToken *nameList, GzPointer *valueList)
/* numParts - how many names and values */
{
/* HW 2.2
-- Pass in a triangle description with tokens and values corresponding to
      GZ_NULL_TOKEN:		do nothing - no values
      GZ_POSITION:		3 vert positions in model space
-- Invoke the rastrizer/scanline framework
-- Return error code
*/
	int minX, minY, maxX, maxY;
	float *p0, *p1, *p2, *pL, *pR, *pG, *pB, *pNx, *pNy, *pNz;
	GzColor color;
	GzCoord norm0, norm1, norm2;
	GzCoord* coord = new GzCoord[3];
	for (int i = 0; i < numParts; i++) {
		switch (nameList[i])
		{
		case GZ_NULL_TOKEN:
			break;
		case GZ_POSITION:
		{
			GzCoord* coords = (GzCoord*)valueList[i];
			// apply stack to vertex
			for (int i = 0; i < 3; i++) {
				GzCoord res;
				if (vectorMultiply(Ximage[matlevel], coords[i], res, GZ_POSITION) == GZ_FAILURE) {
					return GZ_FAILURE;
				}
				coords[i][X] = res[X];
				coords[i][Y] = res[Y];
				coords[i][Z] = res[Z];
			}
			copyCoord(coords[0], coord[0]);
			copyCoord(coords[1], coord[1]);
			copyCoord(coords[2], coord[2]);
			
			break;
		}
		case GZ_NORMAL:
		{
			/* HW4.3 calc shades*/
			GzCoord* norm = (GzCoord*)valueList[i];
			// apply Xn to norms
			for (int i = 0; i < 3; i++) {
				GzCoord res;
				vectorMultiply(Xnorm[matlevel], norm[i], res, GZ_NORMAL);
				norm[i][X] = res[X];
				norm[i][Y] = res[Y];
				norm[i][Z] = res[Z];
			}
			copyCoord(norm[0], norm0);
			copyCoord(norm[1], norm1);
			copyCoord(norm[2], norm2);
			break;
		}
		case GZ_TEXTURE_INDEX:
			break;
		default:
			break;
		}
	}
	// vertex sorting
	float y0 = coord[0][Y], y1 = coord[1][Y], y2 = coord[2][Y];
	int *sort = sortVert(y0, y1, y2);

	float *params = computeExpression(coord[sort[2]], coord[sort[0]]);
	float x = (-params[1] * coord[sort[1]][Y] - params[2]) / params[0];

	if (coord[sort[1]][0] < x) {
		float temp = sort[1];
		sort[1] = sort[2];
		sort[2] = temp;
	}

	// compute plane
	GzCoord e0, e1;
	minusCoord(coord[0], coord[1], e0);
	minusCoord(coord[0], coord[2], e1);
	pL = computePlane(e0, e1, coord[0]);

	/* HW4 Gouraud interpolation */
	if (interp_mode == GZ_COLOR) {
		GzColor c0, c1, c2;
		shadeColor(norm0, c0);
		shadeColor(norm1, c1);
		shadeColor(norm2, c2);

		GzCoord cd0 = { coord[0][X], coord[0][Y], c0[RED] };
		GzCoord cd1 = { coord[1][X], coord[1][Y], c1[RED] };
		GzCoord cd2 = { coord[2][X], coord[2][Y], c2[RED] };
		//compute interpolated RGB
		// R
		minusCoord(cd0, cd1, e0);
		minusCoord(cd0, cd2, e1);
		pR = computePlane(e0, e1, cd0);
		// G
		cd0[2] = c0[GREEN];
		cd1[2] = c1[GREEN];
		cd2[2] = c2[GREEN];
		minusCoord(cd0, cd1, e0);
		minusCoord(cd0, cd2, e1);
		pG = computePlane(e0, e1, cd0);
		// B
		cd0[2] = c0[BLUE];
		cd1[2] = c1[BLUE];
		cd2[2] = c2[BLUE];
		minusCoord(cd0, cd1, e0);
		minusCoord(cd0, cd2, e1);
		pB = computePlane(e0, e1, cd0);
	}

	/* HW4 Phong interpolation */
	if (interp_mode == GZ_NORMALS) {
		GzCoord cd0 = { coord[0][X], coord[0][Y], norm0[X] };
		GzCoord cd1 = { coord[1][X], coord[1][Y], norm1[X] };
		GzCoord cd2 = { coord[2][X], coord[2][Y], norm2[X] };
		//compute interpolated norm
		// normX
		minusCoord(cd0, cd1, e0);
		minusCoord(cd0, cd2, e1);
		pNx = computePlane(e0, e1, cd0);
		// normY
		cd0[2] = norm0[Y];
		cd1[2] = norm1[Y];
		cd2[2] = norm2[Y];
		minusCoord(cd0, cd1, e0);
		minusCoord(cd0, cd2, e1);
		pNy = computePlane(e0, e1, cd0);
		// normZ
		cd0[2] = norm0[Z];
		cd1[2] = norm1[Z];
		cd2[2] = norm2[Z];
		minusCoord(cd0, cd1, e0);
		minusCoord(cd0, cd2, e1);
		pNz = computePlane(e0, e1, cd0);
	}

	// compute A, B, C for 3 edges
	p0 = computeExpression(coord[sort[0]], coord[sort[1]]);
	p1 = computeExpression(coord[sort[1]], coord[sort[2]]);
	p2 = computeExpression(coord[sort[2]], coord[sort[0]]);

	minX = min3(coord[0][X], coord[1][X], coord[2][X]);
	minY = min3(coord[0][Y], coord[1][Y], coord[2][Y]);
	maxX = max3(coord[0][X], coord[1][X], coord[2][X]);
	maxY = max3(coord[0][Y], coord[1][Y], coord[2][Y]);

	minX = inBound(xres, 0, minX);
	minY = inBound(yres, 0, minY);
	maxX = inBound(xres, 0, maxX);
	maxY = inBound(yres, 0, maxY);
	// render
	if (interp_mode == GZ_FLAT) {
		shadeColor(norm0, color);
	}
	for (int j = minY; j <= maxY; j++) {
		for (int i = minX; i <= maxX; i++) {
			if (LEE(p0, i, j) <= 0
				&& LEE(p1, i, j) <= 0
				&& LEE(p2, i, j) < 0) {
				int zPix = (-pL[3] - pL[0] * i - pL[1] * j) / pL[2];
				// ignore z behind view plane
				// ignore larger z
				if (zPix >= 0 && zPix < pixelbuffer[j * xres + i].z) {
					GzIntensity r, g, b;
					if (interp_mode == GZ_FLAT) {
						r = inBound(MAXCOLOR, 0, ctoi(color[0]));
						g = inBound(MAXCOLOR, 0, ctoi(color[1]));
						b = inBound(MAXCOLOR, 0, ctoi(color[2]));
					} else if (interp_mode == GZ_COLOR) {
						float red, green, blue;
						red = (-pR[3] - pR[0] * i - pR[1] * j) / pR[2];
						green = (-pG[3] - pG[0] * i - pG[1] * j) / pG[2];
						blue = (-pB[3] - pB[0] * i - pB[1] * j) / pB[2];
						r = inBound(MAXCOLOR, 0, ctoi(red));
						g = inBound(MAXCOLOR, 0, ctoi(green));
						b = inBound(MAXCOLOR, 0, ctoi(blue));
					} else if (interp_mode == GZ_NORMALS) {
						GzColor color;
						float normX = (-pNx[3] - pNx[0] * i - pNx[1] * j) / pNx[2];
						float normY = (-pNy[3] - pNy[0] * i - pNy[1] * j) / pNy[2];
						float normZ = (-pNz[3] - pNz[0] * i - pNz[1] * j) / pNz[2];
						GzCoord norm = {normX, normY, normZ};
						normalize(norm);
						shadeColor(norm, color);
						r = inBound(MAXCOLOR, 0, ctoi(color[0]));
						g = inBound(MAXCOLOR, 0, ctoi(color[1]));
						b = inBound(MAXCOLOR, 0, ctoi(color[2]));
					}
					pixelbuffer[j * xres + i].red = r;
					pixelbuffer[j * xres + i].green = g;
					pixelbuffer[j * xres + i].blue = b;
					pixelbuffer[j * xres + i].z = zPix;
				}
			}
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

int GzRender::vectorMultiply(GzMatrix m, GzCoord c, GzCoord res, int flag) {
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
	if (res[Z] < 0 && flag == GZ_POSITION) {
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

void GzRender::copyColor(GzColor c0, GzColor c1) {
	// copy color c0 to c1
	c1[RED] = c0[RED];
	c1[GREEN] = c0[GREEN];
	c1[BLUE] = c0[BLUE];
	return;
}
void GzRender::copyCoord(GzCoord c0, GzCoord c1) {
	// copy coord c0 to c1
	c1[X] = c0[X];
	c1[Y] = c0[Y];
	c1[Z] = c0[Z];
	return;
}
void GzRender::minusCoord(GzCoord c0, GzCoord c1, GzCoord res) {
	res[0] = c0[0] - c1[0];
	res[1] = c0[1] - c1[1];
	res[2] = c0[2] - c1[2];
	return;
}
void GzRender::plusCoord(GzCoord c0, GzCoord c1, GzCoord res) {
	res[0] = c0[0] + c1[0];
	res[1] = c0[1] + c1[1];
	res[2] = c0[2] + c1[2];
	return;
}
void GzRender::normalize(GzCoord c) {
	float k = sqrt(pow(c[0], 2) + pow(c[1], 2) + pow(c[2], 2));
	c[0] /= k;
	c[1] /= k;
	c[2] /= k;
	return;
}
/* HW4 compute shade*/
void GzRender::shadeColor(GzCoord norm, GzColor color) {
	GzCoord specular = { 0.0, 0.0, 0.0 };
	GzCoord diffuse = { 0.0, 0.0, 0.0 };
	GzCoord ambient = { 0.0, 0.0, 0.0 };
	for (int i = 0; i < numlights; i++) {
		GzCoord R;
		GzCoord E = {0.0, 0.0, -1.0};
		float nl, re, ne;
		float param;
		nl = dot(norm, lights[i].direction); // N dot L
		ne = dot(norm, E); // N dot E
		if (ne < 0 && nl < 0) {
			norm[0] *= -1.0;
			norm[1] *= -1.0;
			norm[2] *= -1.0;
			nl *= -1.0;
		} else if (ne * nl < 0) {
			continue;
		}
		param = 2 * dot(norm, lights[i].direction);
		R[0] = param * norm[0] - lights[i].direction[0];
		R[1] = param * norm[1] - lights[i].direction[1];
		R[2] = param * norm[2] - lights[i].direction[2];

		re = dot(R, E); // R dot E
		if (re < 0) {
			re = 0;
		}
		specular[0] += lights[i].color[0] * pow(re, spec);
		specular[1] += lights[i].color[1] * pow(re, spec);
		specular[2] += lights[i].color[2] * pow(re, spec);

		diffuse[0] += lights[i].color[0] * nl;
		diffuse[1] += lights[i].color[1] * nl;
		diffuse[2] += lights[i].color[2] * nl;
	}
	specular[0] *= Ks[0];
	specular[1] *= Ks[1];
	specular[2] *= Ks[2];

	diffuse[0] *= Kd[0];
	diffuse[1] *= Kd[1];
	diffuse[2] *= Kd[2];

	ambient[0] = Ka[0] * ambientlight.color[0];
	ambient[1] = Ka[1] * ambientlight.color[1];
	ambient[2] = Ka[2] * ambientlight.color[2];

	color[0] = specular[0] + diffuse[0] + ambient[0];
	color[1] = specular[1] + diffuse[1] + ambient[1];
	color[2] = specular[2] + diffuse[2] + ambient[2];

	color[0] = color[0] > 1.0 ? 1.0 : color[0];
	color[1] = color[1] > 1.0 ? 1.0 : color[1];
	color[2] = color[2] > 1.0 ? 1.0 : color[2];
	return;
}
