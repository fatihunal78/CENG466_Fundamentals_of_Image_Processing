/* * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                      *
 *               Faruk ÖNEN, Fatih ÜNAL                 *
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
#define THRESHOLD (0.3)

FILE *Inf;
FILE *Outf;

int maxv, width, height;
int **rch, **gch, **bch;

int **komsu;	 /* komsu[width*height][2] arrayi*/
int **ziyaret;	 /*ziyaret[width][height] arrayi*/
int **regionno;	/*region[width][height] arrayi*/

/*red green and blue values of the image that is read from input and they are stored in pointer arrays*/
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

/*this values are used in blur sharpen and log functions their alignments are below*/
int deger1;
int deger2;
int deger3;

/*this functions writes to the output file,it firstly writes the row column and max values and unnecessary character values*/
void writefile(FILE *wp)
{
	fprintf(wp, "P3\n# faruk ve fatih\n");
	fprintf(wp, "%d %d\n%d\n", height,width, maxv);
}

/*this function is used to read from the input file,it reads the column row and max values,it skips the unnecessary character values*/
void readfile()
{
	int i=0;
	int j=0;
	int k=0;
	
	/*getting space for input arguments*/
	ppmred=(int *) malloc (height*width*sizeof(int));
	ppmgreen=(int *) malloc (height*width*sizeof(int));
	ppmblue=(int *) malloc (height*width*sizeof(int));
	
	/*reading the inputs for each red green and blue values*/
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			ppmred[k]=rch[i][j];
			ppmgreen[k]=gch[i][j];
			ppmblue[k]=bch[i][j];
			k++;
		}
}

/*this function finds the array index of a given row and column value*/
int rowcolumnfinder(int x1,int y1)
{
	int k;
	k=y1*height+x1;
	return k;
}

/* this function blures the image it uses the mask*/

/* mask is:
 0       1       0
 1       1       1
 0       1       0
*/
void blur()
{
	int i;
	int x,y;
	
	/*temporary variables for calculating the new red green and blue values*/
	int redtotal;
	int greentotal;
	int bluetotal;
	
	/*mask values*/
	deger1=1;
	deger2=1;
	
	/*allocating space*/
	redtotals=(int *) malloc (height*width*sizeof(int));
	greentotals=(int *) malloc (height*width*sizeof(int));
	bluetotals=(int *) malloc (height*width*sizeof(int));
	
	/*copies the values of the input image to temporary array*/
	for(i=0;i<height*width;i++)
	{
		redtotals[i]=ppmred[i];
		greentotals[i]=ppmgreen[i];
		bluetotals[i]=ppmblue[i];
	}
	
	/*does the necessary changes to the copied image by using the mask*/
	for (y=1;y<height-1;y++)
		for(x=1;x<width-1;x++)
		{
			redtotal=0;
			greentotal=0;
			bluetotal=0;
			
			redtotal+=ppmred[rowcolumnfinder(x,y-1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x,y-1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x,y-1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x-1,y)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x-1,y)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x-1,y)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x+1,y)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x+1,y)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x+1,y)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y+1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x,y+1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x,y+1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y)]*deger2;
			greentotal+=ppmgreen[rowcolumnfinder(x,y)]*deger2;
			bluetotal+=ppmblue[rowcolumnfinder(x,y)]*deger2;
			
			/*controling the red green and blue values so as to make them stable between 0 and max boundaries*/
			if(redtotal/9<0)
				redtotal=0;
			if(redtotal/9> maxv)
				redtotal=maxv;
			
			if(greentotal/9<0)
				greentotal=0;
			if(greentotal/9> maxv)
				greentotal=maxv;
			
			if(bluetotal/9<0)
				bluetotal=0;
			if(bluetotal/9> maxv)
				bluetotal=maxv;
			
			redtotals[rowcolumnfinder(x,y)]=redtotal/9;
			greentotals[rowcolumnfinder(x,y)]=greentotal/9;
			bluetotals[rowcolumnfinder(x,y)]=bluetotal/9;
		}
	
	/*writes to the output file from the temporary array*/
	for(i=0;i<height*width;i++)
	{
		fprintf(Outf,"%d %d %d ",redtotals[i],greentotals[i],bluetotals[i]);
	}
}

/*the median filtering function takes the median of 3*3 matrix area of each 9 pixel of the function it navigates through the file from left to right and meanwhile to the bottom*/
void medianfilter()
{
	int i,j;
	int x,y;
	int tmp;
	
	/*allocating space*/
	redtotals=(int *) malloc (height*width*sizeof(int));
	greentotals=(int *) malloc (height*width*sizeof(int));
	bluetotals=(int *) malloc (height*width*sizeof(int));
	
	/*copying the input to the temporary arrays*/
	for(i=0;i<height*width;i++)
	{
		redtotals[i]=ppmred[i];
		greentotals[i]=ppmgreen[i];
		bluetotals[i]=ppmblue[i];
	}
	
	/*calculating the median of each 9 pixel area the 5th of each is called median*/
	for (y=1;y<height-1;y++)
		for(x=1;x<width-1;x++)
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
	for(i=0;i<height*width;i++)
	{
		fprintf(Outf,"%d %d %d ",redtotals[i],greentotals[i],bluetotals[i]);
		
	}
}

/* These two functions will process the image and  write the result to output */
void RegionGrowing()
{
	int i;
	int j;
	
	int x;
	int y;

	int m;
	int n;
	
	int t;
	int r;
	int k;

	int p;
	int sayac=0;

	t=THRESHOLD*maxv;
	
	komsu = (int **) malloc(height*width*sizeof(int *));
	ziyaret = (int **) malloc(height*sizeof(int *));
	regionno = (int **) malloc(height*sizeof(int *));
	
	for (i=0; i<height; i++)
	{
		ziyaret[i] = (int *) malloc(width*sizeof(int));
		regionno[i] = (int *) malloc(width*sizeof(int));
	}
	
	for(i=0 ; i< height*width; i++)
	{
		 komsu[i] = (int *) malloc(2*sizeof(int));
	}
	
	r=0;
	p=0;

for (m = 0; m < height; m++)
{
	for (n = 0; n < width; n++)
	{
		y=m;
		x=n;
		
		for(i=0 ; i< height*width; i++)
		{
			komsu[i][0]=0;
			komsu[i][1]=0;
		}
		
/*	printf("*****%d*****\n",p++);*/
	
	k=0;
	j=0;
	
	sayac=0;
	
	while(ziyaret[y][x]!=2 || k<j)
	{
		sayac=1;
		ziyaret[y][x]=2;
		regionno[y][x]=r;
		
		if(y-1>=0 && x-1>=0)
		{
			if(ziyaret[y-1][x-1]!=1 && ziyaret[y-1][x-1]!=2)
			{		
				if(rch[y][x]+t > rch[y-1][x-1] && rch[y][x]-t < rch[y-1][x-1])
				if(gch[y][x]+t > gch[y-1][x-1] && gch[y][x]-t < gch[y-1][x-1])
				if(bch[y][x]+t > bch[y-1][x-1] && bch[y][x]-t < bch[y-1][x-1])
				{
						komsu[j][0]=y-1;
						komsu[j][1]=x-1;
			/*	printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y-1][x-1]=r;
						ziyaret[y-1][x-1]=1;
				}
			}
		}
		
		if(y-1>=0)
		{
			if(ziyaret[y-1][x]!=1 && ziyaret[y-1][x]!=2)
			{
				if(rch[y][x]+t > rch[y-1][x] && rch[y][x]-t < rch[y-1][x])
				if(gch[y][x]+t > gch[y-1][x] && gch[y][x]-t < gch[y-1][x])
				if(bch[y][x]+t > bch[y-1][x] && bch[y][x]-t < bch[y-1][x])
				{
						komsu[j][0]=y-1;
						komsu[j][1]=x;
			/*	printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y-1][x]=r;
						ziyaret[y-1][x]=1;
				}
			}
		}
		
		if(y-1>=0 && x+1<width)
		{
			if(ziyaret[y-1][x+1]!=1 && ziyaret[y-1][x+1]!=2)
			{
				if(rch[y][x]+t > rch[y-1][x+1] && rch[y][x]-t < rch[y-1][x+1])
				if(gch[y][x]+t > gch[y-1][x+1] && gch[y][x]-t < gch[y-1][x+1])
				if(bch[y][x]+t > bch[y-1][x+1] && bch[y][x]-t < bch[y-1][x+1])
				{
						komsu[j][0]=y-1;
						komsu[j][1]=x+1;
		/*		printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y-1][x+1]=r;
						ziyaret[y-1][x+1]=1;
				}
			}
		}
		
		if(x-1>=0)
		{
			if(ziyaret[y][x-1]!=1 && ziyaret[y][x-1]!=2)
			{
				if(rch[y][x]+t > rch[y][x-1] && rch[y][x]-t < rch[y][x-1])
				if(gch[y][x]+t > gch[y][x-1] && gch[y][x]-t < gch[y][x-1])
				if(bch[y][x]+t > bch[y][x-1] && bch[y][x]-t < bch[y][x-1])
				{
						komsu[j][0]=y;
						komsu[j][1]=x-1;
			/*	printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y][x-1]=r;
						ziyaret[y][x-1]=1;
				}
			}
		}
		
		if(x+1<width)
		{
			if(ziyaret[y][x+1]!=1 && ziyaret[y][x+1]!=2)
			{
				if(rch[y][x]+t > rch[y][x+1] && rch[y][x]-t < rch[y][x+1])
				if(gch[y][x]+t > gch[y][x+1] && gch[y][x]-t < gch[y][x+1])
				if(bch[y][x]+t > bch[y][x+1] && bch[y][x]-t < bch[y][x+1])
				{
						komsu[j][0]=y;
						komsu[j][1]=x+1;
			/*	printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y][x+1]=r;
						ziyaret[y][x+1]=1;
				}
			}
		}
		
		if(x-1>=0 && y+1<height)
		{
			if(ziyaret[y+1][x-1]!=1 && ziyaret[y+1][x-1]!=2)
			{
				if(rch[y][x]+t > rch[y+1][x-1] && rch[y][x]-t < rch[y+1][x-1])
				if(gch[y][x]+t > gch[y+1][x-1] && gch[y][x]-t < gch[y+1][x-1])
				if(bch[y][x]+t > bch[y+1][x-1] && bch[y][x]-t < bch[y+1][x-1])
				{
						komsu[j][0]=y+1;
						komsu[j][1]=x-1;
			/*	printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y+1][x-1]=r;
						ziyaret[y+1][x-1]=1;
				}
			}
		}
		
		if(y+1<height)
		{
			if(ziyaret[y+1][x]!=1 && ziyaret[y+1][x]!=2)
			{
				if(rch[y][x]+t > rch[y+1][x] && rch[y][x]-t < rch[y+1][x])
				if(gch[y][x]+t > gch[y+1][x] && gch[y][x]-t < gch[y+1][x])
				if(bch[y][x]+t > bch[y+1][x] && bch[y][x]-t < bch[y+1][x])
				{
						komsu[j][0]=y+1;
						komsu[j][1]=x;
			/*	printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y+1][x]=r;
						ziyaret[y+1][x]=1;
				}
			}
		}

		if(x+1<width && y+1<height)
		{
			if(ziyaret[y+1][x+1]!=1 && ziyaret[y+1][x+1]!=2)
			{
				if(rch[y][x]+t > rch[y+1][x+1] && rch[y][x]-t < rch[y+1][x+1])
				if(gch[y][x]+t > gch[y+1][x+1] && gch[y][x]-t < gch[y+1][x+1])
				if(bch[y][x]+t > bch[y+1][x+1] && bch[y][x]-t < bch[y+1][x+1])
				{
						komsu[j][0]=y+1;
						komsu[j][1]=x+1;
			/*	printf("$:j:%d y:%d x:%d\n",j,komsu[j][0],komsu[j][1]);*/
						j++;
						regionno[y+1][x+1]=r;
						ziyaret[y+1][x+1]=1;
				}
			}
		}
		
/*		printf("ziyaret: y:%d x:%d s:%d\n",y,x,ziyaret[y][x]);*/
		y=komsu[k][0];
		x=komsu[k][1];
/*		printf("#r:%d k:%d j:%d yeni degerler: y:%d x:%d\n\n",r,k,j,y,x);*/
		k++;
	}

	if(!sayac)
		r; 
	else
		r++;


/*        printf("regionno y:1 x:1 %d\n",regionno[1][1]);*/

	}
}

/*	printf("%d\n",r);*/
	
	fprintf(Outf, "P3\n# faruk ve fatih\n");
	fprintf(Outf, "%d %d\n%d\n", height,width,255);
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
			fprintf(Outf,"%d %d %d ",(regionno[i][j]*20)%255,(regionno[i][j]*20)%255,(regionno[i][j]*20)%255);


}

void SplitAndMerge()
{
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
	Inf = fopen(argv[1], "r");
	Outf = fopen(argv[2], "w");
	
	/* reading input image into arrays ('rch' for R channel, 'gch' for G channel and 'bch' for B channel) */
	
	fscanf(Inf, "P3\n");
	ch = fgetc(Inf);
	if (ch == '#')
		while (ch != '\n')
			ch = fgetc(Inf);
	else ungetc(ch, Inf);
	fscanf(Inf, "%d %d\n%d\n", &height, &width, &maxv);
	
	
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
			fscanf(Inf, "%d %d %d", &rch[i][j],
					&gch[i][j], &bch[i][j]);

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
	
	return 0;
}
