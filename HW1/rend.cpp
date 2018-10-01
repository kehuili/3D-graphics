#include	"stdafx.h"
#include	"stdio.h"
#include	"math.h"
#include	"Gz.h"
#include	"rend.h"
/*   CS580 HW   */
#include    "stdafx.h"  
#include	"Gz.h"

#include	<iostream>
#include	<stdio.h>

GzRender::GzRender(int xRes, int yRes)
{
/* HW1.1 create a framebuffer for MS Windows display:
 -- set display resolution
 -- allocate memory for framebuffer : 3 bytes(b, g, r) x width x height
 -- allocate memory for pixel buffer
 */
	xres = xRes;
	yres = yRes;
	framebuffer = new char[3 * xRes * yRes];
	pixelbuffer = new GzPixel[xRes * yRes];
}

GzRender::~GzRender()
{
/* HW1.2 clean up, free buffer memory */
	delete framebuffer;
	delete pixelbuffer;
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
			pixelbuffer[j * xres + i].z = 0;
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
			uint8_t buffer[3] = {r, g, b}; 
			//std::cout << "test " << std::endl;
			//std::cout << r << g << b;
			fwrite(buffer, 1, 3, outfile);
		}
	}
	if (fclose(outfile)) {
		AfxMessageBox(_T("The output file was not closed\n"));
		return GZ_FAILURE;
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