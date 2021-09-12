/*Fatih Unal 1250885*/
/*Mine Arsoy 1249929*/
/*ceng466 homework 3*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*macro definitions*/
#define MAX_IMAGE_WIDTH 512
#define MAX_IMAGE_HEIGHT 512
#define NUMBER_OF_CHANNELS 3
#define THRESHOLD 0.03
#define ORIENT_SCALE 40.0
#define true 1
#define false 0

typedef int REGISTER;
FILE *Inf;
FILE *Outf;
FILE *Mystdout, *Mystdin;    /*i/o file descriptors*/
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


struct header {
  int rows;
  int cols;
};
struct header hd;
long picsize;

unsigned char *data1, *data2, *data3;    /* input and output array */
 
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
void read_image2(char *image_filename,type_imageCO *image);
void write_image(char *image_filename, type_imageCO *image);
void write_image2(char *image_filename, type_imageCO *image);
void print_usage(void);

/*our own functions*/
void convert_RGB_to_HSV(type_imageCO *image);	
void convert_HSV_to_RGB(type_imageCO *image);
void medianfilter(type_imageCO *image);
int yuvarla(double x);
void split(int x_start, int x_end, int y_start, int y_end);
void makeMask(int size1, int size2, float *arr);
void medianFilter(type_imageCO *image);
void canny_core(double s,int cols,int rows,unsigned char *data,unsigned 
char *derivative_mag,unsigned char *magnitude,unsigned char *orientation);
double gaussian(double x,double s);
void thresholding_tracker(int high,int low,int cols,int rows,unsigned
char *data,unsigned char *magnitude,unsigned char *orientation);
void fwrite_header(FILE * fp, struct header * hd);
int fread_header(FILE * fp, struct header * hd);
double hypotenuse(double x,double y);

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
  fprintf(stdout, "hw3 Q1 <input filename> <output filename>\n\n");
  fprintf(stdout, "hw3 Q2 <input filename> <output filename>\n\n");
}

/*-----------------------------------------------------------------------*/

void question1(char *image_filename, char *output_filename)
{
	type_imageCO image;
	float arr[] = {0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1,0.1};
	
	read_image(image_filename, &image);
	makeMask(3,3,arr);

	medianFilter(&image);

	convert_RGB_to_HSV(&image);
	
	hue_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	saturation_array = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	value_array  = (float*)malloc(sizeof(float)*(width/2 + 1) * (height/2 + 1));
	split(0,width-1,0,height-1);
	
	convert_HSV_to_RGB(&image);

	write_image(output_filename, &image);
	
}

/*-----------------------------------------------------------------------*/

void question2(char *image_filename, char *output_filename)
{
	type_imageCO image;
	read_image2(image_filename, &image);

	
        double s=1.0;            	/* standard deviation of gaussian */
       
        int low=20,high=150;        	/* tracker hysteresis parameters */
     	unsigned char *derivative_mag; /*mag of del G before non-maximum suppression*/
        unsigned char *magnitude,*orientation; /*mag & orient after non-max sppression*/

        derivative_mag=(unsigned char *)calloc(picsize,1);
        magnitude=(unsigned char *)calloc(picsize,1);
        orientation=(unsigned char *)calloc(picsize,1);
	
/* call canny core routines - these perform a gaussian smoothing, calculate a gradient array and suppress non- maximal points in this array */
	canny_core(s,hd.cols,hd.rows,data1,derivative_mag,magnitude,orientation);
	canny_core(s,hd.cols,hd.rows,data2,derivative_mag,magnitude,orientation);
	canny_core(s,hd.cols,hd.rows,data3,derivative_mag,magnitude,orientation);
	
	
/* track edges */
	thresholding_tracker(high,low,hd.cols,hd.rows,data1,magnitude,orientation);
	thresholding_tracker(high,low,hd.cols,hd.rows,data2,magnitude,orientation);
	thresholding_tracker(high,low,hd.cols,hd.rows,data3,magnitude,orientation);
	
	write_image2(output_filename, &image);  
	
}



/*------------------------------canny edge detectýon----------------------------*/

void read_image2(char *image_filename,type_imageCO *image)
{
  
      	int i;	
      	Mystdin = fopen(image_filename,"r");
      	fread_header(Mystdin, &hd);
      	picsize=hd.cols*hd.rows;
	
      	data1=(unsigned char *)malloc(picsize*sizeof(int));
      	data2=(unsigned char *)malloc(picsize*sizeof(int));
      	data3=(unsigned char *)malloc(picsize*sizeof(int));	
	
	for(i = 0; i < picsize; i++) 
	{
		data1[i]=fgetc(Mystdin);
		data2[i]=fgetc(Mystdin);
		data3[i]=fgetc(Mystdin);	                          
	}
	
}

void write_image2(char *output_filename,type_imageCO *image)
{
	Mystdout = fopen(output_filename,"w");
	fwrite_header(Mystdout, &hd);
	int i,j,m=0;	
	for (i = 0; i < hd.rows; i++)
		for (j = 0; j < hd.cols; j++)
		{
			image->pixels[i][j][0]=data1[m];
			image->pixels[i][j][1]=data2[m];
			image->pixels[i][j][2]=data3[m++];
			
		}
	for (i = 0; i < hd.rows; i++)
		for (j = 0; j < hd.cols; j++)
			fprintf(Mystdout,"%c%c%c",image->pixels[i][j][0],image->pixels[i][j][1],image->pixels[i][j][2]);
	
	
}

double hypotenuse(double x,double y)
{
    	if (x==0.0 && y==0.0) return(0.0);
    	else return (hypot(x,y));
}

void canny_core(double s,int cols,int rows,unsigned char *data,unsigned char *derivative_mag,unsigned char *magnitude,unsigned char *orientation)
{
      	int filter_width;               /* length of 1-D gaussian mask */
      	float *gsmooth_x,*gsmooth_y;
      	float *derivative_x,*derivative_y;
      	int i,k,n;            /* counters */
      	int t;                /* temp. grad magnitude variable */
      	double a,b,c,d,g0;        /* mask generation intermediate vars*/
      	double ux,uy;
      	double t1,t2;
      	double G[20],dG[20],D2G[20];    /*gaussian & derivative filter 
					  masks*/
      	double gc,gn,gs,gw,ge,gnw,gne,gsw,gse;
      	int picsize,jstart,jlimit;
      	int ilimit;
      	REGISTER jfactor;
      	int kfactor1,kfactor2;
      	int kfactor;
      	REGISTER cindex,nindex,sindex,windex,eindex,nwindex,neindex,swindex,seindex;
	
      	picsize=cols*rows;        /* picture area */
	
      	/* calc coeffs for 1-dimensional G, dG/dn and
	 *      Delta-squared G filters */
      	for(n=0; n<20; ++n)
    	{
	  	a=gaussian(((double) n),s);
	  	if (a>0.005 || n<2)
		{
	      		b=gaussian((double)n-0.5,s);
	      		c=gaussian((double)n+0.5,s);
	      		d=gaussian((double)n,s*0.5);
			
	       		
	      		G[n]=(a+b+c)/3/(6.283185*s*s);
	      		dG[n]=c-b;
	      		D2G[n]=1.6*d-a; /* DOG */
			
			
		}
	  	else break;
    	}
      	filter_width=n;
	
	
      	/* allocate space for gaussian smoothing arrays */
      	if ((gsmooth_x=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    	{
	  	
	  	exit(0);
    	}
      	if ((gsmooth_y=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    	{
		
	  	exit(0);
    	}
	
      	/* produce x- and y- convolutions with gaussian */
	
      	ilimit=cols-(filter_width-1);
      	jstart=cols*(filter_width-1);
      	jlimit=cols*(rows-(filter_width-1));
      	for (i=filter_width-1;i<ilimit;++i)
    	{
	  	for(jfactor=jstart;
		      		jfactor<jlimit;
		      		jfactor+=cols)
		{
	      		cindex=i+jfactor;
	      		t1=data[cindex]*G[0];
	      		t2=t1;
	      		for(k=1,kfactor1=cindex-cols,kfactor2=cindex+cols;k<filter_width;k++,kfactor1-=cols,kfactor2+=cols)
	    		{
		  		t1+=G[k]*(data[kfactor1]+data[kfactor2]);
		  		t2+=G[k]*(data[cindex-k]+data[cindex+k]);
	    		}
	      		gsmooth_x[cindex]=t1;
	      		gsmooth_y[cindex]=t2;
		}
    	}
    	
      	/* allocate space for gradient arrays */
      	
      	if ((derivative_x=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    	{
	  	exit(0);
    	}
      	/* produce x and y convolutions with derivative of
	 *      gaussian */
	
      	for (i=filter_width-1;i<ilimit;++i)
    	{
	  	for(jfactor=jstart;jfactor<jlimit;jfactor+=cols)
		{
	      		t1=0;
	      		cindex=i+jfactor;
	      		for (k=1;k<filter_width;++k) 
				t1+=dG[k]*(gsmooth_x[cindex-k]-	gsmooth_x[cindex+k]);
	      		derivative_x[cindex]=t1;
		}
    	}
      	free(gsmooth_x);
      	if ((derivative_y=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    	{
	  	exit(0);
    	}
	
      	for (i=n;i<cols-n;++i)
    	{
	  	for(jfactor=jstart;jfactor<jlimit;jfactor+=cols)
		{
	      		t2=0;
	      		cindex=i+jfactor;
	      		for (k=1,kfactor=cols;k<filter_width;k++,kfactor+=cols)
		    		t2+=dG[k]*(gsmooth_y[cindex-kfactor]-gsmooth_y[cindex+kfactor]);
	      		derivative_y[cindex]=t2;
		}
    	}
      	free(gsmooth_y);
    	
      	/* non-maximum suppression (4 cases for orientation of line
	 *      of max slope) */
	
      	ilimit=cols-filter_width;
      	jstart=cols*filter_width;
      	jlimit=cols*(rows-filter_width);
	
      	for (i=filter_width;i<ilimit;++i)
    	{
	  	for (jfactor=jstart;jfactor<jlimit;jfactor+=cols)
		{
			/* calculate current indeces */
	      		cindex=i+jfactor;
	      		nindex=cindex-cols;
	      		sindex=cindex+cols;
	      		windex=cindex-1;
	      		eindex=cindex+1;
	      		nwindex=nindex-1;
	      		neindex=nindex+1;
	      		swindex=sindex-1;
	      		seindex=sindex+1;
	      		ux=derivative_x[cindex];
	      		uy=derivative_y[cindex];
	      		gc=hypotenuse(ux,uy);
	      		/* scale gc to fit into an unsigned char array */
	      		t=(int)(gc*20.0);
			
	      		derivative_mag[cindex]=(t<256 ? t : 255);
	      		gn=hypotenuse(derivative_x[nindex],derivative_y[nindex]);
	      		gs=hypotenuse(derivative_x[sindex],derivative_y[sindex]);
	      		gw=hypotenuse(derivative_x[windex],derivative_y[windex]);
	      		ge=hypotenuse(derivative_x[eindex],derivative_y[eindex]);
	      		gne=hypotenuse(derivative_x[neindex],derivative_y[neindex]);
	      		gse=hypotenuse(derivative_x[seindex],derivative_y[seindex]);
	      		gsw=hypotenuse(derivative_x[swindex],derivative_y[swindex]);
	      		gnw=hypotenuse(derivative_x[nwindex],derivative_y[nwindex]);
	      		if (ux*uy>0)
	    		{
		  		if(fabs(ux)<fabs(uy))                
				{
		      			if((g0=fabs(uy*gc))< fabs(ux*gse+(uy-ux)*gs) ||g0<=fabs(ux*gnw+(uy-ux)*gn))
			    			continue;
				}
		  		else
				{
		      			if((g0=fabs(ux*gc)) < fabs(uy*gse+(ux-uy)*ge) ||g0<=fabs(uy*gnw+(ux-uy)*gw))
			    			continue;
				}
	    		}
	      		else
	    		{
		  		if(fabs(ux)<fabs(uy))
				{
		      			if((g0=fabs(uy*gc))< fabs(ux*gne-(uy+ux)*gn) ||	g0<=fabs(ux*gsw-(uy+ux)*gs))
			    			continue;
				}
		  		else
				{
		      			if((g0=fabs(ux*gc))< fabs(uy*gne-(ux+uy)*ge) ||	g0<=fabs(uy*gsw-(ux+uy)*gw))
			    			continue;
				}
	    		}
	      		/* seems to be a good scale factor */
	      		magnitude[cindex]=derivative_mag[cindex];
	      		/* pi*40 ~= 128 - direction is (thought of as) a signed byte */
	      		orientation[cindex]=(int)(atan2(uy, ux)*ORIENT_SCALE);
		}
    	} 
	
      	free(derivative_x);
      	free(derivative_y);
}

/*            canny_subr.c                */

/* Subroutines used by *canny progs (but not by *canny_j progs) */

/* gaussian function (centred at the origin and ignoring the factor  of 1/(s*sqrt(2*PI)) ) */
double gaussian(double x,double s)
{
    	return(exp((-x*x)/(2*s*s)));
}

/* follow a connected edge */
/* !!! (mh)  added int return type */
int follow(int i,int j,int low,int cols,int rows,unsigned char *data,unsigned char *magnitude,unsigned char *orientation)
{
    	int k,l;        /* counters */
    	int i_plus_1,i_minus_1,j_plus_1,j_minus_1;
    	long index,kindex;
    	char break_flag;
	
    	i_plus_1=i+1;
    	i_minus_1=i-1;
    	j_plus_1=j+1;
    	j_minus_1=j-1;
    	index=i+j*cols;
    	if (j_plus_1>=rows) j_plus_1=rows-1;
    	if (j_minus_1<0) j_minus_1=0;
    	if (i_plus_1>=cols) i_plus_1=cols-1;
    	if (i_minus_1<0) i_minus_1=0;
	
    	if (!data[index])
    	{
		
		/* current point must be added to the list of tracked points */
		data[index]=magnitude[index];
		/* now we can check immediately adjacent points to see if they too could be added to the track */
		break_flag=0;
		for (k=i_minus_1;k<=i_plus_1;k++)
		{
	    		for(l=j_minus_1;l<=j_plus_1;++l)
	    		{
				kindex=k+l*cols;
				if (!(l==j && k==i) &&
			    			magnitude[kindex]>=low)
				{
		    			if 
						(follow(k,l,low,cols,rows,data,magnitude,orientation))
			    			{
							break_flag=1;
							break;
			    			}
				}
	    		}
	    		if (break_flag) break;
		}
		return(1);
    	}
    	else return(0);
}




void thresholding_tracker(int high,int low,int cols,int rows,unsigned char *data,unsigned char *magnitude,unsigned char *orientation)
{
    	int i,j;    /* counters */
    	int picsize;    /* picture area */
	
	
    	/* clear data array before tracking */
    	picsize=cols*rows;
    	for (i=0;i<picsize;++i) data[i]=0;
	
    	/* threshold image with hysteresis: follow from
	 *        each strong edge point */
    	for (i=0;i<cols;++i)
    	{
		for (j=0;j<rows;++j)
	    		if (magnitude[i+cols*j]>=high)
				follow(i,j,low,cols,rows,data,magnitude,orientation);
    	}
}

void fwrite_header(FILE * fp, struct header * hd)
{
	fprintf(fp,"P6\n");
	fprintf(fp,"#Created by us\n");
	fprintf(fp,"%d %d\n%d\n",hd->cols,hd->rows,255);
       	
}


int fread_header(FILE * fp, struct header * hd)
{
	
    	char ch;
	fscanf(fp, "P6\n");
	ch = fgetc(fp);
	if (ch == '#')
		while (ch != '\n')
			ch = fgetc(fp);
	else ungetc(ch, fp);
	fscanf(fp, "%d %d\n%d\n", &hd->cols, &hd->rows, &maxv);
       	
      	return 1;
}



/*--------------------------------------end of canny edge detection---------------------------------*/

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
	
	image->pixels[y][x][0]=(unsigned char) yuvarla(R);
	image->pixels[y][x][1]=(unsigned char) yuvarla(G);
	image->pixels[y][x][2]=(unsigned char) yuvarla(B);
	
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
void merge(int x_start, int x_end, int y_start, int y_end) 
{

	long int current = 0, p = 0;
	int i, j;
	
	
	for(i = y_start; i <= y_end; i++) 
	{
		for(j = x_start; j <= x_end; j++) 
		{
			hue_array[current] = hch[i][j];
			saturation_array[current] = sch[i][j];
			value_array[current++] = vch[i][j];
			
		}
	}

	
	sum1 = 0;
	sum2 = 0;
	sum3 = 0;
	for(p = 0; p < current; p++) 
	{
		sum1 += hue_array[p];
		sum2 += saturation_array[p];
		sum3 += value_array[p];
		
	}
	sum1 /= current;
	sum2 /= current;
	sum3 /= current;

	for(i = y_start; i <= y_end; i++) 
	{
		for(j = x_start; j <= x_end; j++) 
		{
			hch[i][j] = sum1;
			sch[i][j] = sum2;
			vch[i][j] = sum3;
			
		}
	}


}

/*-----------------------------------------------------------------------*/
/*splits image according to its quad tree regions if it is not homogeneous it splits otherwise it merges quad tree regions*/
void split(int x_start, int x_end, int y_start, int y_end) 
{
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

	
	for(i = 0; i < height; i++) 
	{
		for(k = 0; k < width; k++) 
		{
			if((i < seed.y) || ((height - i) < (mask_y  - seed.y))) 
			{
				r = (unsigned char)rch[i][k];
				g = (unsigned char)gch[i][k];
				b = (unsigned char)bch[i][k];
			}
			else if((k < seed.x) || ((width - k) < (mask_x  - seed.x))) 
			{
				r = (unsigned char)rch[i][k];
				g = (unsigned char)gch[i][k];
				b = (unsigned char)bch[i][k];
			}
			else 
			{
				temp = 0;
				r = 0; g = 0; b = 0;
				for(m = -(seed.y); m < (mask_y - seed.y); m++) 
				{
					for(n = -(seed.x); n < (mask_x - seed.x); n++) 
					{
						myTempMaskR[temp] = (unsigned char)rch[i+m][k+n];
						myTempMaskG[temp] = (unsigned char)gch[i+m][k+n];
						myTempMaskB[temp++] = (unsigned char)bch[i+m][k+n];
					}
				}
				for(pass = 1; pass <= 8; pass++) 
				{
					for(j = 0; j <= 7; j++) 
					{
						if(myTempMaskR[j] > myTempMaskR[j+1]) 
						{
							hold = myTempMaskR[j];
							myTempMaskR[j] = myTempMaskR[j+1];
							myTempMaskR[j + 1] = hold;
						}
					}
				}
				for(pass = 1; pass <= 8; pass++) 
				{
					for(j = 0; j <= 7; j++) 
					{
						if(myTempMaskG[j] > myTempMaskG[j+1]) 
						{
							hold = myTempMaskG[j];
							myTempMaskG[j] = myTempMaskG[j+1];
							myTempMaskG[j + 1] = hold;
						}
					}
				}
				for(pass = 1; pass <= 8; pass++) 
				{
					for(j = 0; j <= 7; j++) 
					{
						if(myTempMaskB[j] > myTempMaskB[j+1]) 
						{
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
