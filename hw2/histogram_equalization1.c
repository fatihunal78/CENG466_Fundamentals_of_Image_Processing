/*Fatih Unal 1250885*/
/*Mine Arsoy 1249929*/
/*ceng466 homework 2*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*macro definitions*/
#define MAX_IMAGE_WIDTH 512
#define MAX_IMAGE_HEIGHT 512
#define NUMBER_OF_CHANNELS 3

/*necessary input and output file pointers height,width and maxv of input and output images*/
FILE *Inf;
FILE *Outf;
int maxv, width, height;

/*temporary arrays for RGB and HSI color systems*/
unsigned char **rch, **gch, **bch;
float **hch, **sch, **vch;

/*reference hard coded histogram*/
int ref_histogram[256];

/*two different reference histogram candidates for pdf1 and pdf2*/
int histogram1[256]= {233,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,204,0,0,0,0,0,0,0,0,0,728,0,0,0,0,0,0,0,0,869,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2061,0,0,0,0,0,1350,0,1817,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1770,0,0,0,2996,0,0,0,0,0,0,16,375,0,0,0,1089,0,0,0,0,0,0,269,0,318,0,922,0,0,0,0,0,798,1041,0,0,0,5,0,0,287,0,160,520,0,239,135,0,352,97,0,0,0,825,475,0,900,0,91,0,569,208,0,0,0,0,183,0,788,0,343,304,0,0,135,187,0,1743,154,123,70,692,0,182,345,0,362,0,0,301,615,0,295,198,0,306,0,144,0,284,180,91,103,155,765,0,171,349,280,0,256,176,0,404,0,0,461,115,425,228,0,0,266,550,89,0,285,0,357,178,256,236,0,174,148,60,179,282,351,119,0,200,362,187,99,0,321,84,217,155,101,0,463,438,0,465,179,567};

int histogram2[256]=
{7,6,51,14,74,0,42,100,254,98,0,182,124,0,323,0,38,0,99,125,212,0,0,254,165,133,105,0,0,0,306,0,211,0,0,0,478,0,442,0,0,0,0,85,385,301,0,0,57,158,490,0,0,0,0,0,382,649,0,0,0,328,277,236,0,160,0,84,0,859,0,0,269,172,0,0,0,335,1081,186,0,0,291,501,0,297,0,55,0,0,1142,0,0,122,0,212,0,732,276,0,0,64,671,0,205,260,0,113,0,0,472,215,0,0,0,667,0,246,247,0,151,228,0,0,717,0,0,0,276,0,0,888,0,0,0,0,319,0,635,284,147,346,104,0,0,0,0,682,0,451,219,0,0,0,0,0,1168,119,282,414,358,0,93,121,52,225,978,157,0,182,44,809,88,186,142,371,242,96,135,215,98,100,314,146,103,394,0,207,200,0,418,153,75,236,110,209,158,0,417,45,311,0,70,309,230,0,412,0,0,174,623,231,152,267,141,187,53,215,198,0,110,401,0,0,540,120,256,139,279,146,0,26,614,125,292,346,76,301,452,154,246,159,224,179,319,228,206,196,167,50,33,56,44,502,461,14};



/* type definitions*/
typedef struct
{
  int width;
  int height;
  unsigned char pixels[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT][NUMBER_OF_CHANNELS];
} type_imageCO;

/* function prototypes*/
void question1(char *image_filename, char *output_filename,char * pdf_type);
void read_image(char *image_filename,type_imageCO *image);
void read_image_desired(char *image_filename,type_imageCO *outimage);
void write_image(char *image_filename, type_imageCO *image);
void print_usage(void);

void convert_RGB_to_HSV(type_imageCO *image);	
void convert_HSV_to_RGB(type_imageCO *image);
void RGB_histogram_esitle(unsigned char ** colorchannel);
void HSV_histogram_esitle(float ** colorchannel);
void Equalize();
void enhance1();
void enhance2();
void Specification();
int yuvarla(double x);

/*---------------------------------------------------------------------*/

int main(int argc, char *argv[])
{

	if(strcmp(argv[1], "Q1") == 0)
	{ 
		question1(argv[2],argv[3],argv[4]);
		
	}
	else  print_usage();
	
	return 0;

}

/*---------------------------------------------------------------------*/

void print_usage(void)
{
  fprintf(stdout, "USAGE:\n------\n");
  fprintf(stdout, "hw2 Q1 <input filename> <output filename> <pdf1 | pdf2>\n\n");
}

/*-----------------------------------------------------------------------*/
/*in function question1 we equilize reference histogram ref_histogram, either to histogram1 or histogram2 according to the input given (pdf1 or pdf2) then we call specific functions for these two diffrent cases.*/
void question1(char *image_filename, char *output_filename,char * pdf_type)
{

	int i;
	type_imageCO image;
	read_image(image_filename, &image);	/*reads the input image*/
	
	if (strcmp(pdf_type,"pdf1")==0)		/*checks pdf type*/
	{
		convert_RGB_to_HSV(&image);	/*converts the image to HSI color system*/
		                                                                                                                             for(i=0; i < 256; i++)
        	{
	        	ref_histogram[i]=histogram1[i];
        	}
	
		enhance1();			/*does histogram equalization on HSI system*/
		Specification();		/*does histogram specification on I values*/
		convert_HSV_to_RGB(&image);	/*convertes image to RGB color system again*/
	} 
	
	else	if (strcmp(pdf_type,"pdf2")==0)	/*checks pdf type*/
	{
		enhance2();			/*does histogram equalization on RGB values*/
		convert_RGB_to_HSV(&image);	/*converts the image to HSI color system*/
	
		for(i=0; i < 256; i++)
		{
			ref_histogram[i]=histogram2[i]; 
		}
	
		Specification();		/*does histogram specification on I values*/
		convert_HSV_to_RGB(&image);	/*convertes image to RGB color system again*/
	}	else print_usage();		/*in case of erroneous input commands*/
	
	write_image(output_filename, &image);	/*writes results to the output image*/
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
	fscanf(Inf, "%d %d\n%d\n", &height, &width, &maxv);
	
		
	/* allocating space for hsv arrays.. */
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
	
	
	/* reading input image into arrays ('rch' for R channel, 'gch' for G channel and 'bch' for B channel) */
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

/*converts each pixels' RGB values to HSV equivalent*/
void convert_RGB_to_HSV(type_imageCO *image)
{
	double R,G,B,minRGB,H,S,V,fark;
	int x,y;

for (y = 0; y < height; y++)
	for (x = 0; x < width; x++)
	{

	/*reads the red green and blue values from temporary image structure*/
	R = rch[y][x];
	G = gch[y][x];
	B = bch[y][x];
		
	/* calculating 'value'(intensity=I)*/
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

/*rounds values*/
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

		/* calculates and writes RGB values*/
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

/*equalizes histogram of the image for V (I=intensity) values of the image it works as the formula in the book*/
void HSV_histogram_esitle(float ** colorchannel)
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

/*----------------------------------------------------------------------*/
/*equalizes histogram of the image for each RGB values of the image it works as the formula in the book*/
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

/*--------------------------------------------------------------------------*/

/*HSV_histogram_esitle function is called only once for the V values*/
void enhance1()
{
	HSV_histogram_esitle(vch);  
}

/*----------------------------------------------------------------------------*/

/*RGB_histogram_esitle function is called three times for R,G and B values of the image*/
void enhance2()
{

	RGB_histogram_esitle(rch);	/*red value array*/
	RGB_histogram_esitle(gch);	/*green value array*/
	RGB_histogram_esitle(bch);	/*blue value array*/
}

/*-----------------------------------------------------------------------------*/

void Equalize(float **colorchannel)
{
     unsigned long histogram[256]; 	/* image histogram */
     unsigned long sum_hist[256]; 	/* sum of histogram elements */
     long number_of_pixels;
     float scale_factor; 		/* normalized scale factor */
     unsigned long i; 			/* index variable */
     unsigned long sum; 		/* variable used to increment sum of hist */
     int j;

	number_of_pixels = width*height;
     
/* clear histogram to 0 */
	for(i=0; i<256; i++)     histogram[i]=0;
     

/* calculate histogram */
     
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
	
	for(i=0; i < height; i++)
		for(j=0; j < width; j++)
			colorchannel[i][j]=sum_hist[(int)colorchannel[i][j]];	
	
	
}

/*-----------------------------------------------------------------------------------*/
/*histogram specification function, that specifies an input image according to the given desired histogram*/
void Specification()
{
   int index,i,j,m,LUT[256];
   int histogram[256],ref_cum_histogram[256],cum_histogram[256];

    	for (index=0;index<256;index++)
    	{
    		histogram[index]=0;
		ref_cum_histogram[index]=0;
		cum_histogram[index]=0;
	}	
       
    
    /* calculate histogram of image */
  
	for(i=0; i < height; i++)
		for(j=0; j < width; j++)
		{
			histogram[(int)vch[i][j]]++;	
		}
	 
		
	 
    /*calculate cumulative histogram & desired cumulative histogram   */
   cum_histogram[0]=histogram[0];
   ref_cum_histogram[0]= ref_histogram[0];

	for (index=1;index<256;index++) 
	{ 
       		cum_histogram[index] = histogram[index]+cum_histogram[index-1];
       		ref_cum_histogram[index] = ref_histogram[index]+ref_cum_histogram[index-1];
   	}

   /* make Look-Up Table(LUT) using cum_histogram & ref_cum_histogram */

	for (m=0;m<256;m++) LUT[m]=m;

   	index=0;
	m=0;

	while (index<256) 
	{
       		while (cum_histogram[m]<ref_cum_histogram[index]) 
       		{ 
	   		m++; 
	   		LUT[m]=index; 
		}
       		index++;
   	}

	
	 for(i=0; i < height; i++)
		for(j=0; j < width; j++)
			vch[i][j]=LUT[(int)vch[i][j]];
} 

/*----------------------------------------------------------------------*/

/*writes the final output image to the output file*/
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
