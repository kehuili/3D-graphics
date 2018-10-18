/* Texture functions for cs580 GzLib	*/
#include    "stdafx.h" 
#include	"stdio.h"
#include	"Gz.h"

GzColor	*image=NULL;
int xs, ys;
int reset = 1;

/* Image texture function */
int tex_fun(float u, float v, GzColor color)
{
  unsigned char		pixel[3];
  unsigned char     dummy;
  char  		foo[8];
  int   		i, j;
  FILE			*fd;

  if (reset) {          /* open and load texture file */
    fd = fopen ("texture", "rb");
    if (fd == NULL) {
      fprintf (stderr, "texture file not found\n");
      exit(-1);
    }
    fscanf (fd, "%s %d %d %c", foo, &xs, &ys, &dummy);
    image = (GzColor*)malloc(sizeof(GzColor)*(xs+1)*(ys+1));
    if (image == NULL) {
      fprintf (stderr, "malloc for texture image failed\n");
      exit(-1);
    }

    for (i = 0; i < xs*ys; i++) {	/* create array of GzColor values */
      fread(pixel, sizeof(pixel), 1, fd);
      image[i][RED] = (float)((int)pixel[RED]) * (1.0 / 255.0);
      image[i][GREEN] = (float)((int)pixel[GREEN]) * (1.0 / 255.0);
      image[i][BLUE] = (float)((int)pixel[BLUE]) * (1.0 / 255.0);
    }

    reset = 0;          /* init is done */
	fclose(fd);
  }

/* bounds-test u,v to make sure nothing will overflow image array bounds */
/* determine texture cell corner values and perform bilinear interpolation */
/* set color to interpolated GzColor value and return */
	GzColor* c = new GzColor[4]; // corners A, B, C, D
	int* x = new int[4];
	int* y = new int[4];
	float s, t;
	// scale
	u = u * (xs - 1);
	v = v * (ys - 1);
	// bilinear interpolation
	x[0] = x[3] = floor(u);
	x[1] = x[2] = ceil(u);
	y[0] = y[1] = floor(v);
	y[3] = y[2] = ceil(v);
	for (int i = 0; i < 4; i++) {
		c[i][RED] = image[y[i] * xs + x[i]][RED];
		c[i][GREEN] = image[y[i] * xs + x[i]][GREEN];
		c[i][BLUE] = image[y[i] * xs + x[i]][BLUE];
	}

	s = u - x[0];
	t = v - y[0];
	
	for (int i = 0; i < 3; i++) {
		color[i] = s * t * c[2][i] + (1 - s) * t * c[3][i] +
					s * (1 - t) * c[1][i] + (1 - s) * (1 - t) * c[0][i];
	}
	return GZ_SUCCESS;
}

/* Procedural texture function */
int ptex_fun(float u, float v, GzColor color)
{
	float interval = 1.0 / 6.0;
	int a = u / interval;
	int b = v / interval;
	if (a % 2 == b % 2) {
		color[RED] = .7;
		color[GREEN] = .4;
		color[BLUE] = 0;
	} else {
		color[RED] = 0.2;
		color[GREEN] = 0.2;
		color[BLUE] = 0.5;
	}
	
return		GZ_SUCCESS;
}

/* Free texture memory */
int GzFreeTexture()
{
	if(image!=NULL)
		free(image);
	return GZ_SUCCESS;
}

