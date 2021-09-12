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
FILE *Inf_out;


FILE *Outf;
int maxv, width, height;
int r;

float *hue_array;
float *saturation_array;
float *value_array;
double sum1, sum2, sum3;

unsigned char **rch, **gch, **bch;
unsigned char **rch_out, **gch_out, **bch_out;

float **hch, **sch, **vch;
float **hch_out, **sch_out, **vch_out;
int **regionno;

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
  unsigned char pixels[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT][NUMBER_OF_CHANNELS];
} type_imageCO;

/* function prototypes*/
void question1(char *image_filename, char *output_filename);
void question2(char *image_filename, char *output_filename);
void read_image(char *image_filename,type_imageCO *image,type_imageCO *out_image);
void write_image(char *image_filename, type_imageCO *image);
void print_usage(void);

void convert_RGB_to_HSV(type_imageCO *image,type_imageCO *out_image);	
void convert_HSV_to_RGB(type_imageCO *image);
void RGB_histogram_esitle(unsigned char ** colorchannel);
void HSV_histogram_esitle(float ** colorchannel);
void Equalize();
void enhance();
int yuvarla(double x);

void Specification(/*float ** ref_histogram,float **colorchannel*/);
/*---------------------------------------------------------------------*/

int main(int argc, char *argv[])
{

	if(strcmp(argv[1], "Q1") == 0) 
		question1(argv[2], argv[3]);
	else if(strcmp(argv[1], "Q2") == 0) 
		printf("Q2");
		/*question2(argv[2], argv[3]);*/
	
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
	int i,j;
	type_imageCO image;
	type_imageCO out_image;
	read_image(image_filename, &image,&out_image);
	
	enhance();
	convert_RGB_to_HSV(&image,&out_image);
	
/*	
	hue_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	saturation_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	value_array  = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	
	split(0,width-1,0,height-1);
*/	
	
	
	
	Specification(/*vch_out,vch*/);	
	convert_HSV_to_RGB(&image);
	
	
	
/*	
	regionfinding(&image);
	neighbour();
*/


	write_image(output_filename, &image);

	/*for(i=0;i<height;i++)
		for(j=0;j<width;j++)
			printf("h:%d s:%d v:%lf\n",(int)hch_out[i][j],(int)sch_out[i][j],vch_out[i][j]);*/

}

/*-----------------------------------------------------------------------*/

/*

void question2(char *image_filename, char *output_filename)
{
	type_imageCO image;
	
	read_image(image_filename, &image);

	float arr[] = {0.1,0.1,0.1,0.1,0.2,0.1,0.1,0.1,0.1};
	makeMask(3,3,arr);

	medianFilter(&image);
*/	
/*	
	convert_RGB_to_HSV(&image);
	
	hue_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	saturation_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	value_array  = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	split(0,width-1,0,height-1);

	enhance();	
*/
/*	convert_HSV_to_RGB(&image);   */

/*	regionfinding(&image);
	neighbour();*/
	
/*	write_image(output_filename, &image);   */
	
/*}    */

/*-----------------------------------------------------------------------*/

void read_image(char *image_filename,type_imageCO *image,type_imageCO *out_image)
{
  	char ch;
	int i,j;
	Inf = fopen(image_filename, "r");
	Inf_out= fopen("2ideal.ppm", "r");
	
	
	
	fscanf(Inf_out, "P6\n");
	ch = fgetc(Inf_out);
	if (ch == '#')
		while (ch != '\n')
			ch = fgetc(Inf_out);
	else ungetc(ch, Inf_out);
	fscanf(Inf_out, "%d %d\n%d\n", &height, &width, &maxv);
	
	
	
	
	
	/* reading input image into arrays ('rch' for R channel, 'gch' for G channel and 'bch' for B channel) */
	fscanf(Inf, "P6\n");
	ch = fgetc(Inf);
	if (ch == '#')
		while (ch != '\n')
			ch = fgetc(Inf);
	else ungetc(ch, Inf);
	fscanf(Inf, "%d %d\n%d\n", &height, &width, &maxv);
	
	
	

	
	
	/* allocating space for hsv arrays.. */
	hch = (float **) malloc(height*sizeof(float *));
	sch = (float **) malloc(height*sizeof(float *));
	vch = (float **) malloc(height*sizeof(float *));
	
	hch_out = (float **) malloc(height*sizeof(float *));
	sch_out = (float **) malloc(height*sizeof(float *));
	vch_out = (float **) malloc(height*sizeof(float *));
	
	
	for (i=0; i<height; i++)
	{
		hch[i] = (float *) malloc(width*sizeof(float));
		sch[i] = (float *) malloc(width*sizeof(float));
		vch[i] = (float *) malloc(width*sizeof(float));
		
		hch_out[i] = (float *) malloc(width*sizeof(float));
		sch_out[i] = (float *) malloc(width*sizeof(float));
		vch_out[i] = (float *) malloc(width*sizeof(float));
	}
	
	
	
	
	/* allocating space for rgb arrays.. */
	rch = (unsigned char **) malloc(height*sizeof(unsigned char *));
	gch = (unsigned char **) malloc(height*sizeof(unsigned char *));
	bch = (unsigned char **) malloc(height*sizeof(unsigned char *));
	
	
	rch_out = (unsigned char **) malloc(height*sizeof(unsigned char *));
	gch_out = (unsigned char **) malloc(height*sizeof(unsigned char *));
	bch_out = (unsigned char **) malloc(height*sizeof(unsigned char *));
	
	for (i=0; i<height; i++)
	{
		rch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		gch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		bch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		
		rch_out[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		gch_out[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		bch_out[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
	}
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			{
					rch[i][j]=fgetc(Inf);
					image->pixels[i][j][0]=(unsigned char)rch[i][j];
					
					rch_out[i][j]=fgetc(Inf_out);
					out_image->pixels[i][j][0]=(unsigned char)rch_out[i][j];
				
					gch[i][j]=fgetc(Inf);
					image->pixels[i][j][1]=(unsigned char)gch[i][j];
					
					gch_out[i][j]=fgetc(Inf_out);
					out_image->pixels[i][j][1]=(unsigned char)gch_out[i][j];
					
					bch_out[i][j]=fgetc(Inf_out);
					out_image->pixels[i][j][2]=(unsigned char)bch_out[i][j];
					
					bch[i][j]=fgetc(Inf);
					image->pixels[i][j][2]=(unsigned char)bch[i][j];
			}
	
}
/*-----------------------------------------------------------------------*/

void convert_RGB_to_HSV(type_imageCO *image,type_imageCO *out_image)
{
	/*fonksiyon RGB color modelindeki her bir pixeli HSV color modeline ceviriyor */
	double R,G,B,minRGB,minRGBout,H,S,V,fark,farkout;
	double Rout,Gout,Bout,Hout,Vout,Sout;
	int x,y;

for (y = 0; y < height; y++)
	for (x = 0; x < width; x++)
	{
	/*dosyadan red green blue input argumanlarini okuyor*/

	R = rch[y][x];
	G = gch[y][x];
	B = bch[y][x];
	
/*	R = image->pixels[y][x][0];
	G = image->pixels[y][x][1];
	B = image->pixels[y][x][2];
*/	
	Rout = out_image->pixels[y][x][0];
	Gout = out_image->pixels[y][x][1];
	Bout = out_image->pixels[y][x][2];
	
	/* 'value' degerini hesaplama...*/
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
	
	/*  'saturation' degerini hesaplama... */
	if (V == 0.0)
		S = 0.0;
	else
		S = fark / V;
	

	/* 'hue' degerini hesaplama... */
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
	
	hch[y][x]=H;
	sch[y][x]=S;
	vch[y][x]=V;

	
if (Rout>Gout)
	{
		if (Gout>Bout) {
			minRGBout = Bout;
			Vout = Rout;
		}
		else { /* B>=G */
			minRGBout = Gout;
			if (Rout>Bout) Vout = Rout;
			else Vout = Bout; /* B>=R */
		}
	}
	else { /* G>=R */
		if (Rout>Bout)
		{
			minRGBout = Bout;
			Vout = Gout;
		}
		else
		{ /* B>=R */
			minRGBout = Rout;
			if (Gout>Bout) Vout = Gout;
			else Vout = Bout; /* B>=G */
		}
	}
	farkout = Vout - minRGBout;
	
	/*  'saturation' degerini hesaplama... */
	if (Vout == 0.0)
		Sout = 0.0;
	else
		Sout = farkout / Vout;
	

	/* 'hue' degerini hesaplama... */
	if (Sout == 0.0)
		Hout = 0.0;
	else
	{
		if (Rout == Vout)
			Hout = 60.0 * (Gout-Bout) / farkout;
		else
		{
			if (Gout == Vout)
				Hout=120.0+60.0*(Bout-Rout)/farkout;
			else
				Hout=240.0+60.0*(Rout-Gout)/farkout;
		}
	}
	
	if (Hout<0.0)
		Hout = Hout + 360.0;
	
	hch_out[y][x]=Hout;
	sch_out[y][x]=Sout;
	vch_out[y][x]=Vout;	
		
	
	
	}
	/*printf("hue:%lf saturation:%lf value:%lf\n", hch[y][x],sch[y][x],vch[y][x]);*/
}

/*---------------------------------------------------------------------*/

int yuvarla(double x) { return (int) (x + 0.49); }

/*----------------------------------------------------------------------*/

void convert_HSV_to_RGB(type_imageCO *image)
{
	/*bu fonksiyon HSV color modelindeki herbir pixeli RGB color modeline ceviriyor.*/
	double R,G,B,H,S,V;
	double p,q,t;
	int i;
	double temp_h;
	double frac_h;
	int x,y;

for (y = 0; y < height; y++)
	for (x = 0; x < width; x++)
	{
	/*dosyadan hue saturation ve value degerlerini okuyor.*/
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

		/* red green ve blue hesaplama...*/
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
	
	/*image->pixels[y][x][0]=(unsigned char) yuvarla(R);
	image->pixels[y][x][1]=(unsigned char) yuvarla(G);
	image->pixels[y][x][2]=(unsigned char) yuvarla(B);*/
	rch[y][x]=(unsigned char) yuvarla(R);
	gch[y][x]=(unsigned char) yuvarla(G);
	bch[y][x]=(unsigned char) yuvarla(B);
	
	}
	/*      printf("red:%d green:%d blue:%d\n",rch[y][x],gch[y][x],bch[y][x]);*/
}

/*---------------------------------------------------------------------*/
void HSV_histogram_esitle(float /*unsigned char*/** colorchannel)
{
	double hist[maxv+1], cum_hist[maxv+1], esit_hist[maxv+1];
	int i,j;
	double carpan;
	
	
	for(i=0; i <= maxv; i++)
		hist[i] = cum_hist[i] = 0;
	for(i=0; i < height; i++)
		for(j=0; j < width; j++)
			hist[(int)colorchannel[i][j]]++;
	
	
	cum_hist[0] = hist[0];
	for(i=1; i <= maxv; i++)
		cum_hist[i] = cum_hist[i-1] + hist[i];
	carpan = ((double)(maxv-1)) / ((double) width * height);
	for(i=0; i <= maxv; i++)
		esit_hist[i] = yuvarla(carpan * cum_hist[i]);
	for(i=0; i < height; i++)
		for(j=0; j<width; j++)
			colorchannel[i][j] = esit_hist[(int)colorchannel[i][j]];
	
	
	return;
}


void RGB_histogram_esitle(unsigned char ** colorchannel)
{
	double hist[maxv+1], cum_hist[maxv+1], esit_hist[maxv+1];
	int i,j;
	double carpan;
	
	
	for(i=0; i <= maxv; i++)
		hist[i] = cum_hist[i] = 0;
	for(i=0; i < height; i++)
		for(j=0; j < width; j++)
			hist[colorchannel[i][j]]++;
	
	
	cum_hist[0] = hist[0];
	for(i=1; i <= maxv; i++)
		cum_hist[i] = cum_hist[i-1] + hist[i];
	carpan = ((double)(maxv-1)) / ((double) width * height);
	for(i=0; i <= maxv; i++)
		esit_hist[i] = yuvarla(carpan * cum_hist[i]);
	for(i=0; i < height; i++)
		for(j=0; j<width; j++)
			colorchannel[i][j] = esit_hist[colorchannel[i][j]];
	
	
	return;
}



void enhance()
{

	RGB_histogram_esitle(rch);
	RGB_histogram_esitle(gch);
	RGB_histogram_esitle(bch);

	/*RGB_histogram_esitle(vch);*/
	/*Equalize(vch);  */
	printf("equalýzeden cýktýýýýýýýý");
	/*Specification(vch_out,vch); */
}




void Equalize(float **colorchannel)
{
     unsigned long histogram[256]; /* image histogram */
     unsigned long sum_hist[256]; /* sum of histogram elements */
     long number_of_pixels;
     float scale_factor; /* normalized scale factor */
     unsigned long i; /* index variable */
     unsigned long sum; /* variable used to increment sum of hist */
     number_of_pixels = width*height;
     /* clear histogram to 0 */

     for(i=0; i<256; i++)     histogram[i]=0;

     /* calculate histogram */
     
     /*kapattýmfor(i=0; i<number_of_pixels; i++)
        histogram[m_pImage[i]]++;  */

	 int j;
	for(i=0; i < height; i++)
		for(j=0; j < width; j++)
			histogram[(int)colorchannel[i][j]]++;	 
	 
	 
	 
	 
     /* calculate normalized sum of hist */
    sum = 0;
    scale_factor = 255.0f / number_of_pixels;

    for(i=0; i<256; i++)
    {
        sum += histogram[i];
        sum_hist[i] = (sum * scale_factor) + 0.5f;
    }

    /* transform image using new sum_hist as a LUT */
   /*kapattým for(i=0; i<number_of_pixels; i++)
        m_pImage[i] = sum_hist[m_pImage[i]];   */

	
	for(i=0; i < height; i++)
		for(j=0; j < width; j++)
			colorchannel[i][j]=sum_hist[(int)colorchannel[i][j]];	
	
	
}





void Specification()
{
   int index,x,y,i,j,m,LUT[256];
   int histogram[256],ref_cum_histogram[256],cum_histogram[256],ref_histogram[256];

    for (index=0;index<256;index++){
    	histogram[index]=0;
	ref_cum_histogram[index]=0;
	cum_histogram[index]=0;
	ref_histogram[index]=0;
	
}	

       
    
    /* calculate histogram of image
     /kapattýmfor(i=0; i<number_of_pixels; i++)
         ////////histogram[m_pImage[i]]++;

	 */
	  
	for(i=0; i < height; i++)
		for(j=0; j < width; j++){
			histogram[(int)vch[i][j]]++;	
			ref_histogram[(int)vch_out[i][j]]++; 
/*	 		printf("refhistogram=%lf\n",ref_histogram[(int)vch_out[i][j]]);
			printf("histogram=%lf\n",histogram[(int)vch[i][j]]);
			printf("vch[%d][%d]=%d\n",i,j,(int)vch[i][j]);*/
		}
	 
		
		
		
	   for (index=1;index<256;index++)
	 	  printf("histogram=%d\n",(int) histogram[index]);
	 
	 
    /*calculate cumulative histogram & desired cumulative histogram   */
   cum_histogram[0]=histogram[0];
   ref_cum_histogram[0]= ref_histogram[0];

   for (index=1;index<256;index++) { 
       cum_histogram[index] = histogram[index]+cum_histogram[index-1];
       ref_cum_histogram[index] = ref_histogram[index]+ref_cum_histogram[index-1];
   	 printf("ref_histogram=%d ", ref_histogram[index]);
	 printf("cum_histogram=%d  ", cum_histogram[index]);
	  printf("ref_cum_histogram=%d\n", ref_cum_histogram[index]);
	 
   }

   /* make Look-Up Table(LUT) using cum_histogram & ref_cum_histogram */

    for (m=0;m<256;m++) LUT[m]=m;

   index=0; m=0;
  	printf("cikis\n"); 
   while (index<256) {
       while (cum_histogram[m]<ref_cum_histogram[index]) 
       {
           printf("cum_histogram=%d ",cum_histogram[m]);
	   printf("ref_cum_histogram=%d\n",ref_cum_histogram[m]);
	   
	   m++; 
	   LUT[m]=index; 
	}
       index++;
   }
	printf("cikis\n");
	
    /*kapattýmfor(i=0; i<number_of_pixels; i++)
         m_pImage[i] = (unsigned char)LUT[(int)image[i]];
	 */;
	 
	 for(i=0; i < height; i++)
		for(j=0; j < width; j++)
			vch[i][j]=LUT[(int)vch[i][j]];
	 
	 
	 
} 






/*----------------------------------------------------------------------*/

void write_image(char *image_filename, type_imageCO *image)
{
        int i,j;
	
	Outf = fopen(image_filename, "w");
	fprintf(Outf,"P6\n");
	fprintf(Outf,"#Created by us\n");
	fprintf(Outf,"%d %d\n%d\n",height,width,maxv);

for (i = 0; i < height; i++)
	for (j = 0; j < width; j++)
		fprintf(Outf,"%c%c%c",rch[i][j],gch[i][j],bch[i][j]);
}
