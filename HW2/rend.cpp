#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"

#include	<climits>
#include	<iostream>
#include	<stdio.h>
/***********************************************/
/* HW1 methods: copy here the methods from HW1 */

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
	int imax = INT_MAX;
	for (int i = 0; i < yres; i++) {
		for (int j = 0; j < xres; j++) {
			pixelbuffer[j * xres + i].red = 4095;
			pixelbuffer[j * xres + i].green = 4095;
			pixelbuffer[j * xres + i].blue = 4095;
			pixelbuffer[j * xres + i].alpha = 4095;
			pixelbuffer[j * xres + i].z = imax;
		}
	}
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
			//std::cout << "test " << std::endl;
			//std::cout << r << g << b;
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
			if (i == 41) {
				GzIntensity a = pixelbuffer[j * xres + i].green;
			}
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
						if (zPix < pixelbuffer[j * xres + i].z) {
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
			max= 1;
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