/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                      *
 *               Faruk ÖNEN, Fatih ÜNAL			*
 *                 1298074    1250885			*
 *                METU, Ankara - Turkey			*
 *                                                      *
 *                     CENG 466                         *
 *                      HW - 4                          *
 *         Digital Image Segmentation using             *
 *   "Region Growing" and "Split & Merge" methods       *
 *                                                      *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*      USAGE:
 *
 *      Run the executable with the following arguments:
 *
 *      $ <exec> <input file> <output file> [1|2]
 *
 *      Input and output files will be 'ppm' type images.
 *      If '1' is given as the last argument, Region Growing
 *      algorithm is used for segmentation, if '2' is given
 *      then Split and Merge algorithm is used.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define THRESHOLD (0.03)

FILE *Inf;	/*these are the input and output file pointers that we use */
FILE *Outf;

int maxv, width, height; /*height width and max value of input file*/

int **rch, **gch, **bch; /*red green and blue value arrays for input file*/
int **sonuc;

int **komsu;	 /* is used for inserting neighbourhood of a pixel*/
int **ziyaret;	 /*is used for to examine a pixel whether it is visited before*/
int **regionno;	/*region number of each region*/

int *redaver;	/*array for average red value of each region*/
int *greenaver;	/*array for average green value of each region*/
int *blueaver;	/*array for average blue value of each region*/


/*these variables below is only for the medianfiltering function we will use to obtain better image*/
/*file pointers that are needed*/
FILE *filein,*fileout;

/*characters that are read from the input ppm format picture,its row, column and max
 * values*/
int row,column,max;

/*red green and blue values of the image that is read from input and they are stored
 * in pointer arrays*/
int *ppmred;
int *ppmgreen;
int *ppmblue;

/*last values of red green and blue of each pixel after they are manipulated by functions,they are stored inpointer arrays*/
int *redtotals;
int *greentotals;
int *bluetotals;

/*it is used median filtering function so as to sort the values of the 9 red green and blue */
int heapred[9];
int heapgreen[9];
int heapblue[9];

/* structure for holding each rectangle of the quad tree */
struct Bolum {
	int sy, sx, w, h;	/* start pixels, width and height */
	int mean[3];	/* average R, G and B values for that region */
			/* if homogen => (-1,-1,-1)	*/
			/* if not known => (-2,-2,-2)	*/
	struct Bolum *solust, *sagust, *solalt, *sagalt, *ana;
};

typedef struct Bolum bolum;

bolum * bas;
/**********************************************************************************/

/*these functions are needed to use the median filtering function*/

/*this function finds the array index of a given row and column value*/
int rowcolumnfinder(int x1,int y1)
{
	int k;
	k=y1*row+x1;
	return k;
}

/*this functions writes to the output file,it firstly writes the row column and max values and unnecessary character values*/
void writefile(FILE *wp)
{
	fprintf(wp, "P3\n# faruk ve fatih\n");
	fprintf(wp, "%d %d\n%d\n", row, column, max);
}

/*this function is used to read from the input file,it reads the column row and max values,it skips the unnecessary character values*/
void readfile(FILE *fp)
{
	char ch1;
	int i;
	
	while((ch1=fgetc(fp))!='\n') {}
	ch1=fgetc(fp);
	if (ch1=='#')
	{
		do
		{
			ch1=fgetc(fp);
		}
		while(ch1!='\n');
	}
	else ungetc(ch1,fp);
	fscanf(fp, "%d %d %d", &row, &column, &max);
	
	/*getting space for input arguments*/
	ppmred=(int *) malloc (row*column*sizeof(int));
	ppmgreen=(int *) malloc (row*column*sizeof(int));
	ppmblue=(int *) malloc (row*column*sizeof(int));
	
	/*reading the inputs for each red green and blue values*/
	for(i=0;i<row*column;i++)
	{
		fscanf(fp,"%d", &ppmred[i]);
		fscanf(fp,"%d", &ppmgreen[i]);
		fscanf(fp,"%d", &ppmblue[i]);
	}
}

/*the median filtering function takes the median of 3*3 matrix area of each 9 pixel of the function it navigates through the file from left to right and
 *  * meanwhile to the bottom*/
void medianfilter()
{
	int i,j;
	int x,y;
	int tmp;
	
	/*allocating space*/
	redtotals=(int *) malloc (row*column*sizeof(int));
	greentotals=(int *) malloc (row*column*sizeof(int));
	bluetotals=(int *) malloc (row*column*sizeof(int));
	
	/*copying the input to the temporary arrays*/
	for(i=0;i<row*column;i++)
	{
		redtotals[i]=ppmred[i];
		greentotals[i]=ppmgreen[i];
		bluetotals[i]=ppmblue[i];
	}
	
	/*calculating the median of each 9 pixel area the 5th of each is called median*/
	for (y=1;y<column-1;y++)
		for(x=1;x<row-1;x++)
		{
			heapred[0]=ppmred[rowcolumnfinder(x-1,y-1)];
			heapgreen[0]=ppmgreen[rowcolumnfinder(x-1,y-1)];
			heapblue[0]=ppmblue[rowcolumnfinder(x-1,y-1)];
			
			heapred[1]=ppmred[rowcolumnfinder(x,y-1)];
			heapgreen[1]=ppmgreen[rowcolumnfinder(x,y-1)];
			heapblue[1]=ppmblue[rowcolumnfinder(x,y-1)];
			
			heapred[2]=ppmred[rowcolumnfinder(x+1,y-1)];
			heapgreen[2]=ppmgreen[rowcolumnfinder(x+1,y-1)];
			heapblue[2]=ppmblue[rowcolumnfinder(x+1,y-1)];
			
			heapred[3]=ppmred[rowcolumnfinder(x-1,y)];
			heapgreen[3]=ppmgreen[rowcolumnfinder(x-1,y)];
			heapblue[3]=ppmblue[rowcolumnfinder(x-1,y)];
			
			heapred[4]=ppmred[rowcolumnfinder(x,y)];
			heapgreen[4]=ppmgreen[rowcolumnfinder(x,y)];
			heapblue[4]=ppmblue[rowcolumnfinder(x,y)];
			
			heapred[5]=ppmred[rowcolumnfinder(x+1,y)];
			heapgreen[5]=ppmgreen[rowcolumnfinder(x+1,y)];
			heapblue[5]=ppmblue[rowcolumnfinder(x+1,y)];
			
			heapred[6]=ppmred[rowcolumnfinder(x-1,y+1)];
			heapgreen[6]=ppmgreen[rowcolumnfinder(x-1,y+1)];
			heapblue[6]=ppmblue[rowcolumnfinder(x-1,y+1)];
			
			heapred[7]=ppmred[rowcolumnfinder(x,y+1)];
			heapgreen[7]=ppmgreen[rowcolumnfinder(x,y+1)];
			heapblue[7]=ppmblue[rowcolumnfinder(x,y+1)];
			
			heapred[8]=ppmred[rowcolumnfinder(x+1,y+1)];
			heapgreen[8]=ppmgreen[rowcolumnfinder(x+1,y+1)];
			heapblue[8]=ppmblue[rowcolumnfinder(x+1,y+1)];
			
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
			
			redtotals[rowcolumnfinder(x,y)]=heapred[4];
			greentotals[rowcolumnfinder(x,y)]=heapgreen[4];
			bluetotals[rowcolumnfinder(x,y)]=heapblue[4];
			
		}
	/*writes to the output file from temprary sorted values*/
	for(i=0;i<row*column;i++)
	{
		fprintf(fileout,"%d %d %d ",redtotals[i],greentotals[i],bluetotals[i]);
	}
	
}
/**************************************************************************************/
void bol (bolum* b) {
	int oy, ox;
	b->sagust = (struct Bolum *) malloc(sizeof(struct Bolum));
	b->solalt = (struct Bolum *) malloc(sizeof(struct Bolum));
	b->solust = (struct Bolum *) malloc(sizeof(struct Bolum));
	b->sagalt = (struct Bolum *) malloc(sizeof(struct Bolum));
	oy = b->sy + (b->h)/2;	/* first pixel of the below part */
	ox = b->sx + (b->w)/2;	/* first pixel of the right part */
	
	b->solust->sy = b->sy;		/* Upper left piece */
	b->solust->sx = b->sx;
	b->solust->w = (b->w)/2;
	b->solust->h = (b->h)/2;
	b->solust->mean[0] = -2;
	b->solust->mean[1] = -2;
	b->solust->mean[2] = -2;
	b->solust->solust = NULL;
	b->solust->solalt = NULL;
	b->solust->sagust = NULL;
	b->solust->sagalt = NULL;
	b->solust->ana = b;
	
	b->sagust->sy = b->sy;		/* upper right piece */
	b->sagust->sx = ox ;
	b->sagust->w = b->w - b->solust->w;
	b->sagust->h = b->solust->h;
	b->sagust->mean[0] = -2;
	b->sagust->mean[1] = -2;
	b->sagust->mean[2] = -2;
	b->sagust->solust = NULL;
	b->sagust->solalt = NULL;
	b->sagust->sagust = NULL;
	b->sagust->sagalt = NULL;
	b->sagust->ana = b;
	
	b->solalt->sy = oy ;		/* lower left piece */
	b->solalt->sx = b->sx;
	b->solalt->w = b->solust->w;
	b->solalt->h = b->h - b->solust->h;
	b->solalt->mean[0] = -2;
	b->solalt->mean[1] = -2;
	b->solalt->mean[2] = -2;
	b->solalt->solust = NULL;
	b->solalt->solalt = NULL;
	b->solalt->sagust = NULL;
	b->solalt->sagalt = NULL;
	b->solalt->ana = b;
	
	b->sagalt->sy = oy ; 		/* lower right piece */
	b->sagalt->sx = ox ;
	b->sagalt->w = b->sagust->w;
	b->sagalt->h = b->solalt->h;
	b->sagalt->mean[0] = -2;
	b->sagalt->mean[1] = -2;
	b->sagalt->mean[2] = -2;
	b->sagalt->solust = NULL;
	b->sagalt->solalt = NULL;
	b->sagalt->sagust = NULL;
	b->sagalt->sagalt = NULL;
	b->sagalt->ana = b;
}

void sinirbas(bolum * b) { 
	int i;
	if (b->solalt) {
		sinirbas(b->solust);
		sinirbas(b->sagust);
		sinirbas(b->solalt);
		sinirbas(b->sagalt);
	}
	else {
		for(i = (b->sy) -1; i < (b->sy) + (b->h) -2; i++) {
			sonuc[i][b->sx - 1] = 255;
			sonuc[i][b->sx +(b->w) -2] = 255;
		}
		for(i = (b->sx) -1; i < (b->sx) + (b->w) -2; i++) {
			sonuc[b->sy - 1][i] = 255;
			sonuc[b->sy  +(b->h) -2][i] = 255;
		}
	}
	return;
}

void yaz(bolum* b) {
	if (b) {
		printf("  *** Bolumun sinirlari:\n\t\t(%d, %d)   (%d, %d)\n\t\th: %d\tw: %d\n",
			b->sy, b->sx, b->sy+b->h-1, b->sx+b->w-1, b->h, b->w);
		yaz(b->solust);
		yaz(b->sagust);
		yaz(b->solalt);
		yaz(b->sagalt);
		return ;
	}
	else return;
}


/*this region growing function looks at each pixel one by one and identifys regions*/
void RegionGrowing()
{
	int i;
	int j;
	
	int x;
	int y;

	int m;
	int n;
	
	int t;
	int r=0;
	int k;

	int sayac;
	int loop;

/*calculates the threshold value according to the max value and defined threshold percentage*/
	t=THRESHOLD*maxv;

	/*allocating space for arrays*/
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
	
/*for every pixel in the image we do region detecting*/	
for (m = 0; m < height; m++)
{
	for (n = 0; n < width; n++)
	{
		/*assigning new values*/
		y=m;
		x=n;
		
		/*emptying the neighbourgood array*/
		for(i=0 ; i< height*width; i++)
		{
			komsu[i][0]=0;
			komsu[i][1]=0;
		}
	
	k=0;
	j=0;
	
	/*variables to control the new region and and region values average*/
	sayac=0;
	loop=0;
	
	/*if a pixel never visited before it goes into the loop*/
	while(ziyaret[y][x]!=2)
	{
		/*it is marked as visited and its region number written*/
		ziyaret[y][x]=2;
		regionno[y][x]=r;
		sayac=1;
		
		/*adding that pixel's red green and blue values*/
		redaver[r]+=rch[y][x];
		greenaver[r]+=gch[y][x];
		blueaver[r]+=bch[y][x];
		loop++;
		
/*controlling the neighbourhood of the pixel 8 neighbours(chessboard)*/
/*if a neighbour is in the same region with that pixel,it is put into the neighbour array and marked as put into the neighbourhood array*/
		if(y-1>=0 && x-1>=0 && ziyaret[y-1][x-1]!=1 && ziyaret[y-1][x-1]!=2)
			{		
				if(rch[y][x]+t > rch[y-1][x-1] && rch[y][x]-t < rch[y-1][x-1]
				&& gch[y][x]+t > gch[y-1][x-1] && gch[y][x]-t < gch[y-1][x-1]
				&& bch[y][x]+t > bch[y-1][x-1] && bch[y][x]-t < bch[y-1][x-1])
				{
						komsu[j][0]=y-1;
						komsu[j][1]=x-1;
						j++;
						ziyaret[y-1][x-1]=1;
				}
			}
		
		if(y-1>=0 && ziyaret[y-1][x]!=1 && ziyaret[y-1][x]!=2)
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
		
		if(y-1>=0 && x+1<width && ziyaret[y-1][x+1]!=1 && ziyaret[y-1][x+1]!=2)
			{
				if(rch[y][x]+t > rch[y-1][x+1] && rch[y][x]-t < rch[y-1][x+1]
				&& gch[y][x]+t > gch[y-1][x+1] && gch[y][x]-t < gch[y-1][x+1]
				&& bch[y][x]+t > bch[y-1][x+1] && bch[y][x]-t < bch[y-1][x+1])
				{
						komsu[j][0]=y-1;
						komsu[j][1]=x+1;
						j++;
						ziyaret[y-1][x+1]=1;
				}
			}
		
		if(x-1>=0 && ziyaret[y][x-1]!=1 && ziyaret[y][x-1]!=2)
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
		
		if(x+1<width && ziyaret[y][x+1]!=1 && ziyaret[y][x+1]!=2)
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
		
		if(x-1>=0 && y+1<height && ziyaret[y+1][x-1]!=1 && ziyaret[y+1][x-1]!=2)
			{
				if(rch[y][x]+t > rch[y+1][x-1] && rch[y][x]-t < rch[y+1][x-1]
				&& gch[y][x]+t > gch[y+1][x-1] && gch[y][x]-t < gch[y+1][x-1]
				&& bch[y][x]+t > bch[y+1][x-1] && bch[y][x]-t < bch[y+1][x-1])
				{
						komsu[j][0]=y+1;
						komsu[j][1]=x-1;
						j++;
						ziyaret[y+1][x-1]=1;
				}
			}
		
		if(y+1<height && ziyaret[y+1][x]!=1 && ziyaret[y+1][x]!=2)
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

		if(x+1<width && y+1<height && ziyaret[y+1][x+1]!=1 && ziyaret[y+1][x+1]!=2)
			{
				if(rch[y][x]+t > rch[y+1][x+1] && rch[y][x]-t < rch[y+1][x+1]
				&& gch[y][x]+t > gch[y+1][x+1] && gch[y][x]-t < gch[y+1][x+1]
				&& bch[y][x]+t > bch[y+1][x+1] && bch[y][x]-t < bch[y+1][x+1])
				{
						komsu[j][0]=y+1;
						komsu[j][1]=x+1;
						j++;
						ziyaret[y+1][x+1]=1;
				}
			}
		
/*the next neighbour of the pixel is obtained from neighbourhood array and examines in the loop as a next input*/
		y=komsu[k][0];
		x=komsu[k][1];
		k++;
	}

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

/*printing the values to the output image for each region we look at average values*/	
	fprintf(Outf, "P3\n# faruk ve fatih\n");
	fprintf(Outf, "%d %d\n%d\n", width,height,maxv);
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		fprintf(Outf,"%d %d %d ",redaver[regionno[i][j]],greenaver[regionno[i][j]],blueaver[regionno[i][j]]);

}

void SplitAndMerge()
{
	int i;
	sonuc = (int **)malloc(height*(sizeof(int *)));
	for(i=0; i<height; i++)
		sonuc[i] = (int *) malloc(width*(sizeof(int)));

	bas = (bolum *) malloc(sizeof(bolum));
	bas->sy = 1;
	bas->sx = 1;
	bas->w = width;
	bas->h = height;
	bas->mean[0] = -2;
	bas->mean[1] = -2;
	bas->mean[2] = -2;
	bas->sagust = NULL;
	bas->sagalt = NULL;
	bas->solust = NULL;
	bas->solalt = NULL;
	bas->ana = NULL;
	bol(bas);

	/* this method writes no output since we have not been able to complete it */
}

int main(int argc, char *argv[])
{
	char ch;
	int i,j;
	
	if (argc != 4) 
	{
		printf("Error: wrong number of arguments !...\n");
		return -1;
	}
	
	filein = fopen(argv[1], "r");
	fileout = fopen("tmpfile.ppm", "w");

	readfile(filein);
	writefile(fileout);
	medianfilter();
	
	Inf = fopen("tmpfile.ppm", "r");
	Outf = fopen(argv[2], "w");

/* reading input image into arrays ('rch' for R channel, 'gch' for G channel and 'bch' for B channel) */
	
	fscanf(Inf, "P3\n");
	ch = fgetc(Inf);
	if (ch == '#')
		while (ch != '\n')
			ch = fgetc(Inf);
	else ungetc(ch, Inf);
	fscanf(Inf, "%d %d\n%d\n", &width, &height, &maxv);
	
	
	/* allocating space for arrays.. */
	rch = (int **) malloc(height*sizeof(int *));
	gch = (int **) malloc(height*sizeof(int *));
	bch = (int **) malloc(height*sizeof(int *));
	
	for (i=0; i<height; i++)
	{
		rch[i] = (int *) malloc(width*sizeof(int));
		gch[i] = (int *) malloc(width*sizeof(int));
		bch[i] = (int *) malloc(width*sizeof(int));
	}
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			fscanf(Inf, "%d %d %d", &rch[i][j],&gch[i][j], &bch[i][j]);

	if (argv[3][0] == '1')
	{
		RegionGrowing();
	}
	else if (argv[3][0] == '2')
	{
		SplitAndMerge();
	}
	else
	{
		printf("Error: last argument can only be '1' or '2'\n");
		return -1;
	}

	remove("tmpfile.ppm");

	return 0;
}
