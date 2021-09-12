/*Fatih Unal 1250885*/
/*Mine Arsoy 1249929*/
/*ceng466 homework 1*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*macro definitions*/
#define MAX_IMAGE_WIDTH 512
#define MAX_IMAGE_HEIGHT 512
#define NUMBER_OF_CHANNELS 3
#define THRESHOLD 0.03
#define true 1
#define false 0

FILE *Inf;
FILE *Outf;
int maxv, width, height;
int r; /*number of regions in the image*/

/*temporary arrays for rgb-shv conversions*/
float *hue_array;
float *saturation_array;
float *value_array;
double sum1, sum2, sum3;

/*temporary arrays for image manipulation,region number of any pixel and rgb-hsv hsv-rgb conversations etc*/
unsigned char **rch, **gch, **bch;
float **hch, **sch, **vch;
int **regionno;

/*stuctures and parameters for median filtering function and its mask*/
float **pixel_values, **myMask;

int mask_x, mask_y;
struct seed_type{
	int x;
	int y;
} seed;

typedef struct {
	int r;
	int g;
	int b;
} pixel_color;

/* type definitions*/
typedef struct
{
  int width;
  int height;
  int maxv;
  unsigned char pixels[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT][NUMBER_OF_CHANNELS];
} type_imageCO;

/* function prototypes*/
void question1(char *image_filename, char *output_filename);
void question2(char *image_filename, char *output_filename);
void read_image(char *image_filename,type_imageCO *image);
void write_image(char *image_filename, type_imageCO *image);
void print_usage(void);

/*our own functions*/
void quantization(type_imageCO *image);
void convert_RGB_to_HSV(type_imageCO *image);	
void convert_HSV_to_RGB(type_imageCO *image);
void enhance();
void medianfilter(type_imageCO *image);
int yuvarla(double x);
void split(int x_start, int x_end, int y_start, int y_end);
void neighbour();
void regionfinding(type_imageCO *image);
void makeMask(int size1, int size2, float *arr);
void medianFilter(type_imageCO *image);

/*---------------------------------------------------------------------*/

int main(int argc, char *argv[])
{

	if(strcmp(argv[1], "Q1") == 0) 
		question1(argv[2], argv[3]);
	else if(strcmp(argv[1], "Q2") == 0) 
		question2(argv[2], argv[3]);
	
	return 0;

}

/*---------------------------------------------------------------------*/

void print_usage(void)
{
  fprintf(stdout, "USAGE:\n------\n");
  fprintf(stdout, "hw1 Q1 <input filename> <output filename>\n\n");
  fprintf(stdout, "hw1 Q2 <input filename> <output filename>\n\n");
}

/*-----------------------------------------------------------------------*/

void question1(char *image_filename, char *output_filename)
{
	type_imageCO image;
	read_image(image_filename, &image);
	quantization(&image);
	write_image(output_filename, &image);
}

/*-----------------------------------------------------------------------*/

void question2(char *image_filename, char *output_filename)
{
	int i=0;
	type_imageCO image;
	float arr[] = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
	
	read_image(image_filename, &image);
	makeMask(3,3,arr);
/*for simple pictures median filtering function increases total region number*/
/*because these simple pictures are already smooth it spoils smoothness on the picture's boundary edges*/
/*in complicated ones it reduces noises smoots the image and decreases total region number*/	
/*in the last given picture at tin,Image1.ppm,our median filtering function increases regions but at the */
/*4.ppm and 5.ppm it reduces noices in the middle area of the images,so if you want you can clear the comments*//*below for the median filtering function*/
	i=0;	
/*for(i=0;i<5;i++)
	medianFilter(&image);
*/	
	convert_RGB_to_HSV(&image);
	
	hue_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	saturation_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	value_array  = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	split(0,width-1,0,height-1);
	
	convert_HSV_to_RGB(&image);
	
	regionfinding(&image);
	neighbour();
	write_image(output_filename, &image);
	
}

/*-----------------------------------------------------------------------*/

void read_image(char *image_filename,type_imageCO *image)
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
	
	/* allocating space for hsv arrays.. they are for 1st question rgb-hsv and hsv-rgb conversations*/
	hch = (float **) malloc(height*sizeof(float *));
	sch = (float **) malloc(height*sizeof(float *));
	vch = (float **) malloc(height*sizeof(float *));
	
	for (i=0; i<height; i++)
	{
		hch[i] = (float *) malloc(width*sizeof(float));
		sch[i] = (float *) malloc(width*sizeof(float));
		vch[i] = (float *) malloc(width*sizeof(float));
	}
	
	
	/* allocating space for rgb arrays.. */
	rch = (unsigned char **) malloc(height*sizeof(unsigned char *));
	gch = (unsigned char **) malloc(height*sizeof(unsigned char *));
	bch = (unsigned char **) malloc(height*sizeof(unsigned char *));
	
	for (i=0; i<height; i++)
	{
		rch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		gch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		bch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
	}
	
	/*reads the image both to the structure and to the RGB arrays*/
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			{
					rch[i][j]=fgetc(Inf);
					image->pixels[i][j][0]=(unsigned char)rch[i][j];
				
					gch[i][j]=fgetc(Inf);
					image->pixels[i][j][1]=(unsigned char)gch[i][j];
					
					bch[i][j]=fgetc(Inf);
					image->pixels[i][j][2]=(unsigned char)bch[i][j];
			}
	
}
/*-----------------------------------------------------------------------*/

/*quantizes the image values to 0-16-32-48-64-...-255 each of new red green and blue values of each pixel gets 16 different value*/
/*so the new image gets 16*16*16 (12bit) different values whereas old image gets 256*256*256 (24bit) different values */

void quantization(type_imageCO *image)
{
	int kalanr,kalang,kalanb;
	int i,j;
	
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			kalanr=image->pixels[i][j][0]%16;
			if(image->pixels[i][j][0]<248)
			{
				if(kalanr<8)
					image->pixels[i][j][0]=image->pixels[i][j][0]-kalanr;
				else
					image->pixels[i][j][0]=image->pixels[i][j][0]+16-kalanr;
			}
			else image->pixels[i][j][0]=255;
			
			kalang=image->pixels[i][j][1]%16;
			if(image->pixels[i][j][1]<248)
			{
				if(kalang<8)
					image->pixels[i][j][1]=image->pixels[i][j][1]-kalang;
				else
					image->pixels[i][j][1]=image->pixels[i][j][1]+16-kalang;
			}
			else image->pixels[i][j][1]=255;
			
			kalanb=image->pixels[i][j][2]%16;
			if(image->pixels[i][j][2]<248)
			{
				if(kalanb<8)
					image->pixels[i][j][2]=image->pixels[i][j][2]-kalanb;
				else
					image->pixels[i][j][2]=image->pixels[i][j][2]+16-kalanb;
			}
			else image->pixels[i][j][2]=255;
		}
	
	
}

/*-----------------------------------------------------------------------*/
/*converts each pixels' RGB values to HSV equivalent*/
void convert_RGB_to_HSV(type_imageCO *image)
{
	
	double R,G,B,minRGB,H,S,V,fark;
	int x,y;

for (y = 0; y < height; y++)
	for (x = 0; x < width; x++)
	{
	/*reads the red green and blue values from image structure*/
	R = rch[y][x];
	G = gch[y][x];
	B = bch[y][x];
	
	/* calculating 'value'*/
	if (R>G)
	{
		if (G>B) {
			minRGB = B;
			V = R;
		}
		else { /* B>=G */
			minRGB = G;
			if (R>B) V = R;
			else V = B; /* B>=R */
		}
	}
	else { /* G>=R */
		if (R>B)
		{
			minRGB = B;
			V = G;
		}
		else
		{ /* B>=R */
			minRGB = R;
			if (G>B) V = G;
			else V = B; /* B>=G */
		}
	}
	fark = V - minRGB;
	
	/* calculating 'saturation' value... */
	if (V == 0.0)
		S = 0.0;
	else
		S = fark / V;
	

	/* calculating 'hue' value... */
	if (S == 0.0)
		H = 0.0;
	else
	{
		if (R == V)
			H = 60.0 * (G-B) / fark;
		else
		{
			if (G == V)
				H=120.0+60.0*(B-R)/fark;
			else
				H=240.0+60.0*(R-G)/fark;
		}
	}
	
	if (H<0.0)
		H = H + 360.0;
	
	/*stores HSV values in temporary arrays for further usage*/
	hch[y][x]=H;
	sch[y][x]=S;
	vch[y][x]=V;
	}
}

/*---------------------------------------------------------------------*/

int yuvarla(double x) { return (int) (x + 0.49); }

/*----------------------------------------------------------------------*/
/*converts HSV values to RGB equivalents for each pixel*/
void convert_HSV_to_RGB(type_imageCO *image)
{
	double R,G,B,H,S,V;
	double p,q,t;
	int i;
	double temp_h;
	double frac_h;
	int x,y;

for (y = 0; y < height; y++)
	for (x = 0; x < width; x++)
	{
	/*reads HSV values from temporary arrays*/
	H = hch[y][x];
	S = sch[y][x];
	V = vch[y][x];
	
	if(S==0.0)
	{
		R=V;
		G=V;
		B=V;
	}
	else
	{
		if (H==360.0)
			temp_h=0.0;
		else temp_h=H;
		
		temp_h=temp_h/60;
		i=(int) temp_h;
		frac_h=temp_h - i;
		
		p= V * (1.0 - S);
		q=V*(1.0 - (S * frac_h));
		t=V*(1.0 - (S * (1.0 - frac_h)));

	   /* writes RGB values*/
		switch (i)
		{
			case 0:
				R = V;
				G = t;
				B = p;
				break;
			case 1:
				R = q;
				G = V;
				B = p;
				break;
			case 2:
				R = p;
				G = V;
				B = t;
				break;
			case 3:
				R = p;
				G = q;
				B = V;
				break;
			case 4:
				R = t;
				G = p;
				B = V;
				break;
			case 5:
				R = V;
				G = p;
				B = q;
				break;
		}
	}
	
	rch[y][x]=(unsigned char) yuvarla(R);
	gch[y][x]=(unsigned char) yuvarla(G);
	bch[y][x]=(unsigned char) yuvarla(B);
	
	}
}
/*---------------------------------------------------------------------*/

void write_image(char *image_filename, type_imageCO *image)
{
        int i,j;
	
	Outf = fopen(image_filename, "w");
	fprintf(Outf,"P6\n");
	fprintf(Outf,"#Created by us\n");
	fprintf(Outf,"%d %d\n%d\n",width,height,maxv);

for (i = 0; i < height; i++)
	for (j = 0; j < width; j++)
		fprintf(Outf,"%c%c%c",image->pixels[i][j][0],image->pixels[i][j][1],image->pixels[i][j][2]);
}
/*------------------------------------------------------------------------*/
/*looks at quad tree regions whether it is homogeneous or not*/
int homogeneous(int x_start, int x_end, int y_start, int y_end) 
{
	
int i, j; 
for(i = y_start; i <= y_end; i++) 
{
	for(j = x_start; j <= x_end; j++) 
	{
		if(vch[i][j] < 50 && vch[y_start][x_start] < 50)
		continue;
		else if(vch[i][j] >= 50 && vch[i][j] < 135 && vch[y_start][x_start] >= 50 && vch[y_start][x_start] < 135) 
		{
			if(sch[i][j] < 0.2 && sch[y_start][x_start] < 0.2)
			continue;
			else if(sch[i][j] >= 0.2 && sch[i][j] <= 1 && sch[y_start][x_start] >= 0.2 && sch[y_start][x_start] <= 1) 
		{
			if(hch[i][j] <= 25 && hch[y_start][x_start] <= 25)
			continue;
			else if(hch[i][j] > 335 && hch[y_start][x_start] > 335)
			continue;	
			else if(hch[i][j] > 25 && hch[i][j] <=50 && hch[y_start][x_start] > 25 && hch[y_start][x_start] <= 50) 
			continue;
			else if(hch[i][j] > 50 && hch[i][j] <=75 && hch[y_start][x_start] > 50 && hch[y_start][x_start] <= 75)
			continue;
			else if(hch[i][j] > 50 && hch[i][j] <=165 && hch[y_start][x_start] > 75 && hch[y_start][x_start] <= 165)
			continue;
			else if(hch[i][j] > 165 && hch[i][j]<=270 && hch[y_start][x_start] > 165 && hch[y_start][x_start] <= 270) 
			continue;
			else if(hch[i][j] > 270 && hch[i][j] <=295 && hch[y_start][x_start] > 270 && hch[y_start][x_start] <= 295)
			continue;
			else if(hch[i][j] > 295 && hch[i][j] <=335 && hch[y_start][x_start] > 295 && hch[y_start][x_start] <= 335)
			continue;
			else
			return false;
		}
		else
			return false;
		}
		
		else if(vch[i][j] >= 135 && vch[i][j] <= 255 && vch[y_start][x_start] >= 135 && vch[y_start][x_start]<= 255) 
		{
			if(sch[i][j] < 0.2 && sch[y_start][x_start] < 0.2)
			continue;
			else if(sch[i][j] >= 0.2 && sch[i][j] <= 1 && sch[y_start][x_start] >= 0.2 && sch[y_start][x_start] <= 1) 
		{
	
			if(hch[i][j] <= 25 && hch[y_start][x_start] <= 25) 
			continue;
			else if(hch[i][j] > 335 && hch[y_start][x_start] > 335)
			continue;
			else if(hch[i][j] > 25 && hch[i][j]<=50 && hch[y_start][x_start] > 25 && hch[y_start][x_start] <= 50) 
			continue;
			else if(hch[i][j] > 50 && hch[i][j] <=75 && hch[y_start][x_start] > 50 && hch[y_start][x_start] <= 75) 
			continue;
			else if(hch[i][j] > 50 && hch[i][j] <=165 && hch[y_start][x_start] > 75 && hch[y_start][x_start] <= 165) 
			continue;
			else if(hch[i][j] > 165 && hch[i][j] <=270 && hch[y_start][x_start] > 165 && hch[y_start][x_start] <= 270) 
			continue;
			else if(hch[i][j] > 270 && hch[i][j] <=295 && hch[y_start][x_start] > 270 && hch[y_start][x_start] <= 295) 
			continue;
			else if(hch[i][j] > 295 && hch[i][j] <=335 && hch[y_start][x_start] > 295 && hch[y_start][x_start] <= 335) 
			continue;
		else
			return false;
				
		}
		else
			return false;

		}
		else
			return false;
		
	}
		
}
	return true;
}
/*------------------------------------------------------------*/
/*merging splitted quad trees*/
void merge(int x_start, int x_end, int y_start, int y_end) {

	long int current = 0, p = 0;
	int i, j;
	
	
	for(i = y_start; i <= y_end; i++) {
		for(j = x_start; j <= x_end; j++) {
			hue_array[current] = hch[i][j];
			saturation_array[current] = sch[i][j];
			value_array[current++] = vch[i][j];
			
		}
	}

	
	sum1 = 0;
	sum2 = 0;
	sum3 = 0;
	for(p = 0; p < current; p++) {
		sum1 += hue_array[p];
		sum2 += saturation_array[p];
		sum3 += value_array[p];
		
	}
	sum1 /= current;
	sum2 /= current;
	sum3 /= current;

	for(i = y_start; i <= y_end; i++) {
		for(j = x_start; j <= x_end; j++) {
			hch[i][j] = sum1;
			sch[i][j] = sum2;
			vch[i][j] = sum3;
			
		}
	}


}

/*-----------------------------------------------------------------------*/
/*splits image according to its quad tree regions if it is not homogeneous it splits otherwise it merges quad tree regions*/
void split(int x_start, int x_end, int y_start, int y_end) {
	if(homogeneous(x_start, x_start + (x_end - x_start)/2, y_start,y_start + (y_end - y_start)/2))
	{
		merge(x_start, x_start + (x_end - x_start)/2, y_start, y_start + (y_end - y_start)/2);
	}
		else
	{	 
		split(x_start, x_start + (x_end - x_start)/2, y_start, y_start + (y_end - y_start)/2);
	}
	
	if(homogeneous(x_start, x_start + (x_end - x_start)/2, y_start + (y_end - y_start)/2 + 1, y_end))
	{
		merge(x_start, x_start + (x_end - x_start)/2, y_start + (y_end - y_start)/2 + 1, y_end);
	}
	else
	{
		split(x_start, x_start + (x_end - x_start)/2, y_start + (y_end - y_start)/2 + 1, y_end);
	}
	
	if(homogeneous(x_start + (x_end - x_start)/2 + 1, x_end, y_start, y_start + (y_end - y_start)/2))
	{
		merge(x_start + (x_end - x_start)/2 + 1, x_end, y_start, y_start + (y_end - y_start)/2);
	}
	else
	{
		split(x_start + (x_end - x_start)/2 + 1, x_end, y_start, y_start + (y_end - y_start)/2);
	}
	
	if(homogeneous(x_start + (x_end - x_start)/2 + 1, x_end, y_start + (y_end - y_start)/2 + 1, y_end))
	{
		merge(x_start + (x_end - x_start)/2 + 1, x_end, y_start + (y_end - y_start)/2 + 1, y_end);
	}
	else
	{
		split(x_start + (x_end - x_start)/2 + 1, x_end, y_start + (y_end - y_start)/2 + 1, y_end);
	}
}

/*---------------------------------------------------------------------------------*/
/*this region finding function is used after extracting quad-trees of an image and this function looks at each pixel one by one and identifys regions region numbers*/
void regionfinding(type_imageCO *image)
{
	int **komsu;     /* is used for inserting neighbourhood of a pixel*/
	int **ziyaret;   /*is used for to examine a pixel whether it is visited before*/
	
	int *redaver;   /*array for average red value of each region*/
	int *greenaver; /*array for average green value of each region*/
	int *blueaver;  /*array for average blue value of each region*/
	
	int i;
	int j;
	
	int x;
	int y;
	
	int m;
	int n;
	
	int t;
	int k;
	
	int sayac;
	int loop;
	
	/*calculates the threshold value according to the max value and defined threshold percentage*/
	t=THRESHOLD*maxv;
	r=0;
	
	/*allocating space for temporary arrays*/
	komsu = (int **) malloc(height*width*sizeof(int *));
	ziyaret = (int **) malloc(height*sizeof(int *));
	regionno = (int **) malloc(height*sizeof(int *));
	
	redaver=(int *) malloc (height*width*sizeof(int));
	greenaver=(int *) malloc (height*width*sizeof(int));
	blueaver=(int *) malloc (height*width*sizeof(int));
	
	for (i=0; i<height; i++)
	{
		ziyaret[i] = (int *) malloc(width*sizeof(int));
		regionno[i] = (int *) malloc(width*sizeof(int));
	}
	
	for(i=0 ; i< height*width; i++)
	{
		komsu[i] = (int *) malloc(2*sizeof(int));
	}
	
	/*resetting the array that shows whether a pixel is visited*/
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			ziyaret[i][j]=0;
	
	/*for every pixel in the image we do region detecting*/
	for (m = 0; m < height; m++)
	{
		for (n = 0; n < width; n++)
		{
			/*assigning new values*/
			y=m;
			x=n;
			
			j=0;
			k=0;
			
			/*variables to control the new region and and region values average*/
			sayac=0;
			loop=0;
			
			/*if a pixel never visited before it goes into the loop*/
			if(ziyaret[y][x]==0)
			{
				do
				{	/*assigns region number*/
					regionno[y][x]=r;
					sayac=1;
					
					/*adding that pixel's red green and blue values*/
					redaver[r]+=rch[y][x];
					greenaver[r]+=gch[y][x];
					blueaver[r]+=bch[y][x];
					loop++;
					
/*controlling the neighbourhood of the pixel 4 neighbours (up down left and right),if we look 8 neighbourhood, we wouldnt be able to detect cross line boundaries*/

/* - our point*/
/*0 X 0*/ 
/*X - X*/
/*0 X 0*/

/*if a neighbour is in the same region with that pixel,it is put into the neighbour array and marked as put into the neighbourhood array*/
					
		/*right*/	if(x+1<width && ziyaret[y][x+1]!=1 && ziyaret[y][x+1]!=2)
					{
						if(rch[y][x]+t > rch[y][x+1] && rch[y][x]-t < rch[y][x+1]
							&& gch[y][x]+t > gch[y][x+1] && gch[y][x]-t < gch[y][x+1]
							&& bch[y][x]+t > bch[y][x+1] && bch[y][x]-t < bch[y][x+1])
						{
							komsu[j][0]=y;
							komsu[j][1]=x+1;
							j++;
							ziyaret[y][x+1]=1;
						}
					}
					
			/*down*/	if(y+1<height &&  ziyaret[y+1][x]!=1 && ziyaret[y+1][x]!=2)
					{
						if(rch[y][x]+t > rch[y+1][x] && rch[y][x]-t < rch[y+1][x]
							&& gch[y][x]+t > gch[y+1][x] && gch[y][x]-t < gch[y+1][x]
							&& bch[y][x]+t > bch[y+1][x] && bch[y][x]-t < bch[y+1][x])
						{
							komsu[j][0]=y+1;
							komsu[j][1]=x;
							j++;
							ziyaret[y+1][x]=1;
						}
					}
					
			/*left*/	if(x-1>=0 && ziyaret[y][x-1]!=1 && ziyaret[y][x-1]!=2)
					{
						if(rch[y][x]+t > rch[y][x-1] && rch[y][x]-t < rch[y][x-1]
							&& gch[y][x]+t > gch[y][x-1] && gch[y][x]-t < gch[y][x-1]
							&& bch[y][x]+t > bch[y][x-1] && bch[y][x]-t < bch[y][x-1])
						{
							komsu[j][0]=y;
							komsu[j][1]=x-1;
							j++;
							ziyaret[y][x-1]=1;
						}
					}
					
			/*up*/		if(y-1>=0 && ziyaret[y-1][x]!=1 && ziyaret[y-1][x]!=2)
					{
						if(rch[y][x]+t > rch[y-1][x] && rch[y][x]-t < rch[y-1][x]
							&& gch[y][x]+t > gch[y-1][x] && gch[y][x]-t < gch[y-1][x]
							&& bch[y][x]+t > bch[y-1][x] && bch[y][x]-t < bch[y-1][x])
						{	
							komsu[j][0]=y-1;
							komsu[j][1]=x;
							j++;
							ziyaret[y-1][x]=1;
						}
					}
					
/*the next neighbour of the pixel is obtained from neighbourhood array and examines in the loop as a next input*/
					
					ziyaret[y][x]=2; /*pixel is marked as visited*/
					
					if(j>=k)/*controls the neighbour array whether it is empty or not*/
					{
						y=komsu[k][0];
						x=komsu[k][1];
						k++;
					}else break;
					
					
				}
				while(ziyaret[y][x]==1);
				
				/*average red green and blue value of each region is calculated*/
				if(loop && sayac)
				{
					redaver[r]/=loop;
					greenaver[r]/=loop;
					blueaver[r]/=loop;
				}
				
				/*examining a region is finished so we pass to the next region*/
				if(sayac)
					r++;
			}
			
		}
	}
	
	/*printing the values to the output image for each region we look at average values*/
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			image->pixels[i][j][0]=redaver[regionno[i][j]];
			image->pixels[i][j][1]=greenaver[regionno[i][j]];
			image->pixels[i][j][2]=blueaver[regionno[i][j]];
		}
}

/*-------------------------------------------------------------------*/
/*function that is used for printing region adjacency graph*/
void neighbour()
{
	
	int x,y;
	int i;
	char **regionadjacent;
	
	/*allocating space for region adjacency matrix*/
	regionadjacent = (char **) malloc(r*sizeof(char *));
	for (i=0; i<r; i++)
	{
		regionadjacent[i] = (char *) malloc(r*sizeof(char));
	}
	
	/*resetting the adjacency matrix*/
	for(y=0;y<r;y++)
		for(x=0;x<r;x++)
		{
			regionadjacent[y][x]='0';
		}
/*used for learning each pixels region number and coordinate*/	
/*	for(y=0;y<height;y++)
                for(x=0;x<width;x++)
			printf("%d %d %d\n", x ,y ,regionno[y][x]);
*/

/*horizontally scans each pixel and looks at adjacency of regions,for neighbour regions it puts 1 else 0, for same regions it puts dash*/
	for(y=0;y<height;y++)
                for(x=0;x<width-1;x++)
                {
                        if (regionno[y][x]!=regionno[y][x+1])
                        {
                                regionadjacent[regionno[y][x+1]][regionno[y][x]]='1';
                                regionadjacent[regionno[y][x]][regionno[y][x+1]]='1';
                        }
			else
			{
                                regionadjacent[regionno[y][x+1]][regionno[y][x]]='-';
                                regionadjacent[regionno[y][x]][regionno[y][x+1]]='-';
                        }
		}

/*vertically scans each pixel and looks at adjacency of regions,for neighbour regions it puts 1 else 0, for same regions it puts dash*/
	for(y=0;y<height-1;y++)
		for(x=0;x<width;x++)
		{
			if(regionno[y][x]!=regionno[y+1][x])
			{
				regionadjacent[regionno[y+1][x]][regionno[y][x]]='1';
                                regionadjacent[regionno[y][x]][regionno[y+1][x]]='1';
			}
			else
                        {
                                regionadjacent[regionno[y][x]][regionno[y+1][x]]='-';
                                regionadjacent[regionno[y+1][x]][regionno[y][x]]='-';
                        }
		}

/*print outs region adjacency matrix*/
	printf("\t");
	for(x=0;x<r;x++)
		printf("R%d\t",x);
	printf("\n");
	
	for(y=0;y<r;y++)
	{
		printf("R%d\t",y);
		for(x=0;x<r;x++)
		{
			printf("%c\t",regionadjacent[y][x]);
		}
		printf("\n");
	}
	
}
/*-----------------------------------------------------------------------*/
/*multi purpose mask*/
void makeMask(int size1, int size2, float *arr) {
	int temp = 0, i, k;
	mask_x = size1;
	mask_y = size2;
	seed.x = size1 / 2;
	seed.y = size2 / 2;
	myMask = (float**)malloc(sizeof(float*)*mask_x);
	for(i = 0; i < mask_y; i++)
		*(myMask + i) = (float*)malloc(sizeof(float)*mask_y);
	for(i = 0; i < size1; i++) {
		for(k = 0; k < size2; k++) {
			myMask[i][k] = arr[temp++];
		}
	}
}
/*-----------------------------------------------------------------------*/
/*median filtering function*/
void medianFilter(type_imageCO *image) {
	int r, g, b, i, k, m, n, temp;
	int myTempMaskR[9];
	int myTempMaskG[9];
	int myTempMaskB[9];
	int hold, pass, j;

	
	for(i = 0; i < height; i++) {
		for(k = 0; k < width; k++) {
			if((i < seed.y) || ((height - i) < (mask_y  - seed.y))) {
				r = (unsigned char)rch[i][k];
				g = (unsigned char)gch[i][k];
				b = (unsigned char)bch[i][k];
			}
			else if((k < seed.x) || ((width - k) < (mask_x  - seed.x))) {
				r = (unsigned char)rch[i][k];
				g = (unsigned char)gch[i][k];
				b = (unsigned char)bch[i][k];
			}
			else {
				temp = 0;
				r = 0; g = 0; b = 0;
				for(m = -(seed.y); m < (mask_y - seed.y); m++) {
					for(n = -(seed.x); n < (mask_x - seed.x); n++) {
						myTempMaskR[temp] = (unsigned char)rch[i+m][k+n];
						myTempMaskG[temp] = (unsigned char)gch[i+m][k+n];
						myTempMaskB[temp++] = (unsigned char)bch[i+m][k+n];
					}
				}
				for(pass = 1; pass <= 8; pass++) {
					for(j = 0; j <= 7; j++) {
						if(myTempMaskR[j] > myTempMaskR[j+1]) {
							hold = myTempMaskR[j];
							myTempMaskR[j] = myTempMaskR[j+1];
							myTempMaskR[j + 1] = hold;
						}
					}
				}
				for(pass = 1; pass <= 8; pass++) {
					for(j = 0; j <= 7; j++) {
						if(myTempMaskG[j] > myTempMaskG[j+1]) {
							hold = myTempMaskG[j];
							myTempMaskG[j] = myTempMaskG[j+1];
							myTempMaskG[j + 1] = hold;
						}
					}
				}
				for(pass = 1; pass <= 8; pass++) {
					for(j = 0; j <= 7; j++) {
						if(myTempMaskB[j] > myTempMaskB[j+1]) {
							hold = myTempMaskB[j];
							myTempMaskB[j] = myTempMaskB[j+1];
							myTempMaskB[j + 1] = hold;
						}
					}
				}
				r = myTempMaskR[4];
				g = myTempMaskG[4];
				b = myTempMaskB[4];
			}
			if(r < 0)
				r = 0;
			if(r > maxv)
				r = maxv;
			if(g < 0)
				g = 0;
			if(g > maxv)
				g = maxv;
			if(b < 0)
				b = 0;
			if(b > maxv)
				b = maxv;
			rch[i][k] = r;
			gch[i][k] = g;
			bch[i][k] = b;

		}
	}

}
