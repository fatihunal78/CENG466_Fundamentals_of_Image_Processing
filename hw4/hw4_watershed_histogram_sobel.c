/*Fatih Unal 1250885*/
/*Mine Arsoy 1249929*/
/*ceng466 homework 4*/
                                                                                                                                                                      
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define SOBEL_THRESHOLD 150
                                                                                                                                                                      
/*macro definitions*/
#define MAX_IMAGE_WIDTH 512
#define MAX_IMAGE_HEIGHT 512
#define NUMBER_OF_CHANNELS 3
#define THRESHOLD 0.03
                                                                                                                                                                      
FILE *Inf;
FILE *Outf;
int maxv, width, height;
int regionno;
int q;

                                                                                                                                                                      
/*temporary arrays for image manipulation,region number of any pixel 
and rgb-hsv hsv-rgb conversations etc*/
unsigned char **rch, **gch, **bch;
float **hch,**sch,**vch;
int **regno;


typedef struct
{
  int width;
  int height;
  int maxv;
  unsigned char 
pixels[MAX_IMAGE_WIDTH][MAX_IMAGE_HEIGHT][NUMBER_OF_CHANNELS];
} type_imageCO;
                                                                                                                                                                      
/* function prototypes*/
void question1(char *image_filename, char *output_filename);
void question2(char *image_filename, char *output_filename);
void read_image(char *image_filename,type_imageCO *image);
void write_image(char *image_filename, type_imageCO *image);
void print_usage(void);
void medianfilter(type_imageCO *image);
void RGB_histogram_esitle(unsigned char ** colorchannel);
void convert_RGB_to_HSV(type_imageCO *image);
void watershed();
int yuvarla(double x) { return (int) (x + 0.49); }

/*******************************************SOBEL RELATED 
DECLERATyONS********************************/
typedef struct
{
int width;
int height;
int color_number;
} type_imageCO_sobel;

int sobelR(type_imageCO_sobel *image);
int sobelG(type_imageCO_sobel *image);
int sobelB(type_imageCO_sobel *image);
int **image1_R;
int **image1_G;
int **image1_B;
int **image2_R;
int **image2_G;
int **image2_B;
void read_imageSobel(char *image_filename, type_imageCO_sobel *image);
void write_imageSobel(char *image_filename, type_imageCO_sobel *image);
void sobel(char *image_filename, char *output_filename);


/*---------------------------------------------------------------------*/
                                                                                                                                                                      
int main(int argc, char *argv[])
{
        if(strcmp(argv[1], "Q1") == 0)
                question1(argv[2], argv[3]);
        else if(strcmp(argv[1], "Q2") == 0)
                question2(argv[2], argv[3]);
        else if (strcmp(argv[1], "sobel") == 0){
		sobel(argv[2], argv[3]);
	
	}                                                                                                                                                       
        return 0;
                                                                                                                                                                      
}
                                                                                                                                                                      
/*---------------------------------------------------------------------*/
                                                                                                                                                                      
void print_usage(void)
{
  fprintf(stdout, "USAGE:\n------\n");
  fprintf(stdout, "hw1 Q1 <input filename> <output filename>\n\n");
  fprintf(stdout, "hw1 Q2 <input filename> <output filename>\n\n");
}

/*--------------------------------------------------------------------*/
void question1(char *image_filename, char *output_filename)
{
	q=1;
	type_imageCO image;
	read_image(image_filename, &image);
	
	convert_RGB_to_HSV(&image);
	
	watershed();
	write_image(output_filename, &image);
	
}
/*---------------------------------------------------------------------*/
void question2(char *image_filename, char *output_filename)
{
	q=2;
        type_imageCO image;
    
	read_image(image_filename, &image);

	RGB_histogram_esitle(rch);
	RGB_histogram_esitle(gch);
	RGB_histogram_esitle(bch);
	
	int i,j;
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			{
					image.pixels[i][j][0]=(unsigned char)rch[i][j];
					image.pixels[i][j][1]=(unsigned char)gch[i][j];
					image.pixels[i][j][2]=(unsigned char)bch[i][j];
			}	
	
	
	
	medianfilter(&image); 	   
	write_image(output_filename, &image);
                                                                                                                                                   
										
}       
/*---------------------------------------------------------*/

void sobel(char *image_filename, char *output_filename){

	type_imageCO_sobel image;
	read_imageSobel(image_filename,&image);
	sobelR(&image);
	sobelB(&image);
	sobelG(&image);
	write_imageSobel(output_filename,&image);


}

/*----------------------------------------------------------------*/

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
	
	/* allocating space for hsv arrays.. they are for 1st question rgb-hsv 
and hsv-rgb conversations*/
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
	
	regno = (int **) malloc(height*sizeof(int *));
	
	for (i=0; i<height; i++)
	{
		rch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		gch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		bch[i] = (unsigned char *) malloc(width*sizeof(unsigned char));
		
		regno[i] = (int *) malloc(width*sizeof(int));		                
	}
	
	/*reads the image both to the structure and to the RGB arrays*/
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			regno[i][j]=-1;	
		
			rch[i][j]=fgetc(Inf);
			image->pixels[i][j][0]=(unsigned char)rch[i][j];
			
			gch[i][j]=fgetc(Inf);
			image->pixels[i][j][1]=(unsigned char)gch[i][j];
			
			bch[i][j]=fgetc(Inf);
			image->pixels[i][j][2]=(unsigned char)bch[i][j];
		}
	
}

/*------------------------------------------------------------------------*/

void write_image(char *image_filename, type_imageCO *image)
{
        int i,j;
                                                                                                                                                                      
        Outf = fopen(image_filename, "w");
        fprintf(Outf,"P6\n");
        fprintf(Outf,"#Created by us\n");
        fprintf(Outf,"%d %d\n%d\n",width,height,maxv);

	if(q==1)
	{
		for (i = 0; i < height; i++)
			for (j = 0; j < width; j++)
			{
				fprintf(Outf,"%c%c%c",rch[i][j],gch[i][j],bch[i][j]);
			}
	}


	if(q==2)
	{	
		for (i = 0; i < height; i++)
			for (j = 0; j < width; j++)
				
fprintf(Outf,"%c%c%c",image->pixels[i][j][0],image->pixels[i][j][1],image->pixels[i][j][2]);
	}
}
 
/*------------------------------------------------------------------------------*/
										
void medianfilter(type_imageCO *image)
{
	
	int heapred[9];
	int heapgreen[9];
	int heapblue[9];
	
	int i,j;
	int x,y;
	int tmp;
	/*calculating the median of each 9 pixel area the 5th of each is 
called median*/
	
	for (x=1;x<height-1;x++)
		for(y=1;y<width-1;y++)
		{
			heapred[0]=rch[x-1][y-1];
			heapgreen[0]=gch[x-1][y-1];
			heapblue[0]=bch[x-1][y-1];
			
			
			heapred[1]=rch[x][y-1];
			heapgreen[1]=gch[x][y-1];
			heapblue[1]=bch[x][y-1];
			
			
			heapred[2]=rch[x+1][y-1];
			heapgreen[2]=gch[x+1][y-1];
			heapblue[2]=bch[x+1][y-1];
			
			
			heapred[3]=rch[x-1][y];
			heapgreen[3]=gch[x-1][y];
			heapblue[3]=bch[x-1][y];
			
			
			heapred[4]=rch[x][y];
			heapgreen[4]=gch[x][y];
			heapblue[4]=bch[x][y];
			
			
			heapred[5]=rch[x+1][y];
			heapgreen[5]=gch[x+1][y];
			heapblue[5]=bch[x+1][y];
			
			
			heapred[6]=rch[x-1][y+1];
			heapgreen[6]=gch[x-1][y+1];
			heapblue[6]=bch[x-1][y+1];
			
			
			heapred[7]=rch[x][y+1];
			heapgreen[7]=gch[x][y+1];
			heapblue[7]=bch[x][y+1];
			
			
			heapred[8]=rch[x+1][y+1];
			heapgreen[8]=gch[x+1][y+1];
			heapblue[8]=bch[x+1][y+1];
			
			
			/*sorts the arrays*/
			for(i=0;i<8;i++)
			{
				for(j=8;j>i;j--)
					if(heapred[j]<heapred[j-1])
					{
						tmp=heapred[j];
						heapred[j]=heapred[j-1];
						heapred[j-1]=tmp;
					}
			}
			

                        for(i=0;i<8;i++)
			{
				for(j=8;j>i;j--)
					if(heapgreen[j]<heapgreen[j-1])
					{
						tmp=heapgreen[j];
						heapgreen[j]=heapgreen[j-1];
						heapgreen[j-1]=tmp;
					}
			}
			
			
			for(i=0;i<8;i++)
			{
				for(j=8;j>i;j--)
					if(heapblue[j]<heapblue[j-1])
					{
						tmp=heapblue[j];
						heapblue[j]=heapblue[j-1];
						heapblue[j-1]=tmp;
					}
			}
			
			rch[x][y]=heapred[4];
			gch[x][y]=heapgreen[4];
			bch[x][y]=heapblue[4];
						
			image->pixels[x][y][0]=heapred[8]- heapred[0];
			image->pixels[x][y][1]=heapgreen[8]-heapgreen[0];
			image->pixels[x][y][2]=heapblue[8]-heapblue[0];
			
			
		}
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
/***********************************************rgb to 
hsv**********************************************************************/

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

/***********************************************Watershed 
segmentation**********************************************************/


void watershed()
{
	int i,j,k;
	int control=0;
	int c=0;
	int no=0;
	int cum=0;
	int count=0;
	int aver=0;
	
regionno=255;

/*by using I value we are segmenting regions*/	
	for(k=255;k>=0;k--)
	{
	        for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				control=0;
				no=0;
				c=0;
				if((int)hch[i][j]==k);
				{
					/*right*/if(j<width-1 && regno[i][j+1]!=-1 && regno[i][j]==-1)
					{
						control=1;
						no+=regno[i][j+1];
						c++;
					}
					
					
					/*down*/if(i<height-1 && regno[i+1][j]!=-1 && regno[i][j]==-1)
					{
						control=1;
						no+=regno[i+1][j];
						c++;
					}
					
					/*left*/if(j>0 && regno[i][j-1]!=-1 && regno[i][j]==-1)
					{
						control=1;
						no+=regno[i][j-1];
						c++;
					}
					
					/*up*/if(i>0 && regno[i-1][j]!=-1 && regno[i][j]==-1)
					{
						control=1;
						no+=regno[i-1][j];
						c++;
					}
					
					if((int)vch[i][j]==k && control==0 && regno[i][j]==-1)
					{
						regno[i][j]=regionno;
					}
					
					
					if((int)vch[i][j]==k && control==1 && regno[i][j]==-1)
					{
						regno[i][j]=no/c;
					}
				}
			}
		}
		regionno--;
	}

	for(k=0;k<256;k++)
	{
		cum=0;
		count=0;
		aver=0;
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if(regno[i][j]==k)
				{
					cum+=rch[i][j];
					count++;
				}
			}
		}
		
		if(count)
			aver=cum/count;
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if(regno[i][j]==k)
				{
					rch[i][j]=aver;
				}
			}
		}
	}
	
	for(k=0;k<256;k++)
	{
		cum=0;
		count=0;
		aver=0;
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if(regno[i][j]==k)
				{
					cum+=gch[i][j];
					count++;
				}
			}
		}
		if(count)
			aver=cum/count;
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if(regno[i][j]==k)
				{
					gch[i][j]=aver;
				}
			}
		}
	}


	for(k=0;k<256;k++)
	{
		cum=0;
		count=0;
		aver=0;
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if(regno[i][j]==k)
				{
					cum+=bch[i][j];
					count++;
				}
			}
		}
		
		if(count)
			aver=cum/count;
		
		for (i = 0; i < height; i++)
		{
			for (j = 0; j < width; j++)
			{
				if(regno[i][j]==k)
				{
					bch[i][j]=aver;
				}
			}
		}
	}

}

/***********************************************SOBEL 
IMPLEMENTATyON************************************************************/



int sobelR(type_imageCO_sobel *image)
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


int sobelG(type_imageCO_sobel *image)
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

int sobelB(type_imageCO_sobel *image)
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


void read_imageSobel(char *image_filename, type_imageCO_sobel *image)
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

void write_imageSobel(char *image_filename, type_imageCO_sobel *image)
{
	int i,j;
	
	Outf = fopen(image_filename, "w");
	fprintf(Outf,"P6\n");
	fprintf(Outf,"#Created by us\n");
	fprintf(Outf,"%d %d\n%d\n",width,height,maxv);
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			if((image2_R[i][j]+image2_G[i][j]+image2_B[i][j])/3 > 
SOBEL_THRESHOLD)
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

