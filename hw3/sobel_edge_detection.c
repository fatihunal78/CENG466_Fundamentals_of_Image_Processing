#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <math.h>
#define THRESHOLD 150

typedef struct
{
int width;
int height;
int color_number;
} type_imageCO;

void question1(char *input_image,char *output_image);
void read_image(char *image_filename, type_imageCO *image);
void write_image(char *image_filename, type_imageCO *image);
int sobelR(type_imageCO *image);
int sobelG(type_imageCO *image);
int sobelB(type_imageCO *image);

FILE *Inf;
FILE *Outf;
int width,height,maxv;

int **image1_R;
int **image1_G;
int **image1_B;
int **image2_R;
int **image2_G;
int **image2_B;

int main(int argc, char *argv[])
{
		question1(argv[1],argv[2]);
	
	return 0;	

}

int sobelR(type_imageCO *image)
{ /*sobel edge detectorfor R channel*/
    int i,j,xres,yres,maxintensity,c,cc;
    xres=image->width;
    yres=image->height;
    maxintensity=image->color_number;

	for (i=1;i<yres-1;i++)
		for (j=1;j<xres-1;j++){
			cc=-image1_R[i-1][j-1]-2*image1_R[i][j-1]-image1_R[i+1][j-1];
			cc+=image1_R[i-1][j+1]+2*image1_R[i][j+1]+image1_R[i+1][j+1];
			c=abs(cc);
			cc=-image1_R[i-1][j-1]-2*image1_R[i-1][j]-image1_R[i-1][j+1];
			cc+=image1_R[i+1][j-1]+2*image1_R[i+1][j]+image1_R[i+1][j+1];
			c+=abs(cc);
			if(c>255) c=255;
			
			image2_R[i][j]=(int) c;		
		}
return 0;
}


int sobelG(type_imageCO *image)
{  /*sobel edge detectorfor G channel*/
    int i,j,xres,yres,maxintensity,c,cc;
    xres=image->width;
    yres=image->height;
    maxintensity=image->color_number;

	for (i=1;i<yres-1;i++)
		for (j=1;j<xres-1;j++){
			cc=-image1_G[i-1][j-1]-2*image1_G[i][j-1]-image1_G[i+1][j-1];
			cc+=image1_G[i-1][j+1]+2*image1_G[i][j+1]+image1_G[i+1][j+1];
			c=abs(cc);
			cc=-image1_G[i-1][j-1]-2*image1_G[i-1][j]-image1_G[i-1][j+1];
			cc+=image1_G[i+1][j-1]+2*image1_G[i+1][j]+image1_G[i+1][j+1];
			c+=abs(cc);
			if(c>255) c=255;

			image2_G[i][j]=(int) c;		
		}
return 0;
}

int sobelB(type_imageCO *image)
{  /*sobel edge detectorfor B channel*/
    int i,j,xres,yres,maxintensity,c,cc;
    xres=image->width;
    yres=image->height;
    maxintensity=image->color_number;

	for (i=1;i<yres-1;i++)
		for (j=1;j<xres-1;j++){
			cc=-image1_B[i-1][j-1]-2*image1_B[i][j-1]-image1_B[i+1][j-1];
			cc+=image1_B[i-1][j+1]+2*image1_B[i][j+1]+image1_B[i+1][j+1];
			c=abs(cc);
			cc=-image1_B[i-1][j-1]-2*image1_B[i-1][j]-image1_B[i-1][j+1];
			cc+=image1_B[i+1][j-1]+2*image1_B[i+1][j]+image1_B[i+1][j+1];
			c+=abs(cc);
			if(c>255) c=255;
			
			image2_B[i][j]=(int) c;		
		}
return 0;
}



void question1(char *input_image,char *output_image)
{
	type_imageCO image;
	read_image(input_image,&image);
	sobelR(&image);
	sobelB(&image);
	sobelG(&image);
	write_image(output_image,&image);
}


void read_image(char *image_filename, type_imageCO *image)
{
	char ch;
	int i,j;
	Inf = fopen(image_filename, "r");
	
	
	/* reading input image header heoght width and max values */
	fscanf(Inf, "P6\n");
	ch = fgetc(Inf);
	if (ch == '#')
		
		while (ch != '\n')
			ch = fgetc(Inf);
	else ungetc(ch, Inf);
       	fscanf(Inf, "%d %d\n%d\n", &width, &height, &maxv);

	image->width=width;
	image->height=height;
	image->color_number=maxv;

	image1_R = (int **) malloc(height*sizeof(int *));
	image1_G = (int **) malloc(height*sizeof(int *));
	image1_B = (int **) malloc(height*sizeof(int *));
	image2_R = (int **) malloc(height*sizeof(int *));
	image2_G = (int **) malloc(height*sizeof(int *));
	image2_B = (int **) malloc(height*sizeof(int *));
	
	for (i=0; i<height; i++)
	{
		image1_R[i] = (int *) malloc(width*sizeof(int));
		image1_G[i] = (int *) malloc(width*sizeof(int));
		image1_B[i] = (int *) malloc(width*sizeof(int));
		image2_R[i] = (int *) malloc(width*sizeof(int));
		image2_G[i] = (int *) malloc(width*sizeof(int));
		image2_B[i] = (int *) malloc(width*sizeof(int));
	}
	
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			image1_R[i][j]=(int)fgetc(Inf);
			image1_G[i][j]=(int)fgetc(Inf);
			image1_B[i][j]=(int)fgetc(Inf);
		}
	
	
}

void write_image(char *image_filename, type_imageCO *image)
{
	int i,j;
	
	Outf = fopen(image_filename, "w");
	fprintf(Outf,"P6\n");
	fprintf(Outf,"#Created by us\n");
	fprintf(Outf,"%d %d\n%d\n",width,height,maxv);
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			if((image2_R[i][j]+image2_G[i][j]+image2_B[i][j])/3 > THRESHOLD)
			{
				image2_R[i][j]=255;
				image2_G[i][j]=255;
				image2_B[i][j]=255;
			}
			else
			{
				image2_R[i][j]=0;
				image2_G[i][j]=0;
				image2_B[i][j]=0;
			}
				
		fprintf(Outf,"%c%c%c",(unsigned char)image2_R[i][j],
				(unsigned char) image2_G[i][j],(unsigned char) image2_B[i][j]);
		}
}
