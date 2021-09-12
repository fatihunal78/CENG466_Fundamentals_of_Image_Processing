/*FATIH UNAL & FARUK ONEN*/
/*1250885		1298074*/
/*IMAGE HOMEWORK PART A*/

#include<stdio.h>
#include <stdlib.h>

FILE *filein,*fileout;	/*file pointers that are needed*/
FILE *Maskfile;
char Operationtype;

/*characters that are read from the input ppm format picture,its row, column and max values*/
char ch1;
int row,column,max;

/*red green and blue values of the image that is read from input and they are stored in pointer arrays*/
int *ppmred;
int *ppmgreen;
int *ppmblue;

/*last values of red green and blue of each pixel after they are manipulated by functions,they are stored inpointer arrays*/
int *redtotals;
int *greentotals;
int *bluetotals;

/*needed necessary variables*/
int posx,posy;
int i,j;
int x,y;
int tmp;

/*it is used median filtering function so as to sort the values of the 9 red green and blue */
int heapred[9];
int heapgreen[9];
int heapblue[9];

/*this values are used in blur sharpen and log functions their alignments are below*/
int deger1;
int deger2;
int deger3;

/*
for blur and sharpen:

0	deger1	0
deger1	deger2	deger1
0	deger1	0


for log:

0	0	deger1	0	0
0	deger1	deger2	deger1	0
deger1	deger2	deger3	deger2	deger1
0	deger1	deger2	deger1	0
0	0	deger1	0	0

*/

/*this function finds the row and column position of a given array index*/
void xyfinder(int a)
{
	posy=a/column;
	posx=a%column;
}

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


/* this function blures the image it uses the mask*/
/*
mask is:
0	1	0
1	1	1
0	1	0
*/
void blur()
{
/*temporary variables for calculating the new red green and blue values*/
	int redtotal;
	int greentotal;
	int bluetotal;

/*mask values*/	
	deger1=1;
	deger2=1;

/*allocating space*/	
	redtotals=(int *) malloc (row*column*sizeof(int));
	greentotals=(int *) malloc (row*column*sizeof(int));
	bluetotals=(int *) malloc (row*column*sizeof(int));
	
/*copies the values of the input image to temporary array*/	
	for(i=0;i<row*column;i++)
	{
		redtotals[i]=ppmred[i];
		greentotals[i]=ppmgreen[i];
		bluetotals[i]=ppmblue[i];
	}

/*does the necessary changes to the copied image by using the mask*/
	for (y=1;y<column-1;y++)
		for(x=1;x<row-1;x++)
		{
			redtotal=0;
			greentotal=0;
			bluetotal=0;
			
			redtotal+=ppmred[rowcolumnfinder(x-1,y-1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x-1,y-1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x-1,y-1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y-1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x,y-1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x,y-1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x+1,y-1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x+1,y-1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x+1,y-1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x-1,y)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x-1,y)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x-1,y)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y)]*deger2;
			greentotal+=ppmgreen[rowcolumnfinder(x,y)]*deger2;
			bluetotal+=ppmblue[rowcolumnfinder(x,y)]*deger2;
			
			redtotal+=ppmred[rowcolumnfinder(x+1,y)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x+1,y)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x+1,y)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x-1,y+1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x-1,y+1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x-1,y+1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y+1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x,y+1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x,y+1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x+1,y+1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x+1,y+1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x+1,y+1)]*deger1;
			
/*controling the red green and blue values so as to make them stable between 0 and max boundaries*/				
			if(redtotal/9<0)
				redtotal=0;
			if(redtotal/9> max)
				redtotal=max;
			
			if(greentotal/9<0)
				greentotal=0;
			if(greentotal/9> max)
				greentotal=max;
			
			if(bluetotal/9<0)
				bluetotal=0;
			if(bluetotal/9> max)
				bluetotal=max;
			
			redtotals[rowcolumnfinder(x,y)]=redtotal/9;
			greentotals[rowcolumnfinder(x,y)]=greentotal/9;
			bluetotals[rowcolumnfinder(x,y)]=bluetotal/9;
		}

/*writes to the output file from the temporary array*/	
	for(i=0;i<row*column;i++)
	{
		fprintf(fileout,"%d %d %d ",redtotals[i],greentotals[i],bluetotals[i]);
	}
}

/*this function sharpens the image*/
void sharpen()
{

/*temporary variables for calculating the new red green and blue values*/
	int redtotal;
	int greentotal;
	int bluetotal;

/*the mask used for the sharpen function*/
/*
the mask is:
0	-1	0
-1	5	-1
0	-1	0
*/
	deger1=-1;
	deger2=5;

/*allocating space for pointer arrays*/	
	redtotals=(int *) malloc (row*column*sizeof(int));
	greentotals=(int *) malloc (row*column*sizeof(int));
	bluetotals=(int *) malloc (row*column*sizeof(int));
	
/*copies the input values from the original input to the temporary array*/
	for(i=0;i<row*column;i++)
	{
		redtotals[i]=ppmred[i];
		greentotals[i]=ppmgreen[i];
		bluetotals[i]=ppmblue[i];
	}

/*change the necessary parts for sharpening*/	
	for (y=1;y<column-1;y++)
		for(x=1;x<row-1;x++)
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
		if(redtotal<0)
			redtotal=0;
		if(redtotal> max)
			redtotal=max;
			
		if(greentotal<0)
			greentotal=0;
		if(greentotal> max)
			greentotal=max;

		if(bluetotal<0)
			bluetotal=0;
		if(bluetotal> max)
			bluetotal=max;		

		redtotals[rowcolumnfinder(x,y)]=redtotal;
		greentotals[rowcolumnfinder(x,y)]=greentotal;
		bluetotals[rowcolumnfinder(x,y)]=bluetotal;
		
		}

/*writes the temporary arrays to the output file*/
	for(i=0;i<row*column;i++)
	{
		fprintf(fileout,"%d %d %d ",redtotals[i],greentotals[i],bluetotals[i]);
		
	}
}

/*the median filtering function takes the median of 3*3 matrix area of each 9 pixel of the function it navigates through the file from left to right and meanwhile to the bottom*/
void medianfilter()
{
	
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

/*log function that uses the mask that is a 5x5 matrice*/
/*mask is:\
0	0	-1	0	0
0	-1	-2	-1	0
-1	-2	16	-2	-1
0	-1	-2	-1	0
0	0	-1	0	0
*/
void Log()
{
/*temporary variables */
	int redtotal;
	int greentotal;
	int bluetotal;

/*mask values*/	
	deger1=-1;
	deger2=-2;
	deger3=16;

/*allocating space*/	
	redtotals=(int *) malloc (row*column*sizeof(int));
	greentotals=(int *) malloc (row*column*sizeof(int));
	bluetotals=(int *) malloc (row*column*sizeof(int));

/*copying input variables into the temporary arrays*/	
	for(i=0;i<row*column;i++)
	{
		redtotals[i]=ppmred[i];
		greentotals[i]=ppmgreen[i];
		bluetotals[i]=ppmblue[i];
	}

/*masking the 5x5 matrice to the image*/	
	for (y=2;y<column-2;y++)
		for(x=2;x<row-2;x++)
		{
			redtotal=0;
			greentotal=0;
			bluetotal=0;
			
			redtotal+=ppmred[rowcolumnfinder(x,y-2)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x,y-2)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x,y-2)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x-1,y-1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x-1,y-1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x-1,y-1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y-1)]*deger2;
			greentotal+=ppmgreen[rowcolumnfinder(x,y-1)]*deger2;
			bluetotal+=ppmblue[rowcolumnfinder(x,y-1)]*deger2;
			
			redtotal+=ppmred[rowcolumnfinder(x+1,y-1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x+1,y-1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x+1,y-1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x-2,y)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x-2,y)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x-2,y)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x-1,y)]*deger2;
			greentotal+=ppmgreen[rowcolumnfinder(x-1,y)]*deger2;
			bluetotal+=ppmblue[rowcolumnfinder(x-1,y)]*deger2;
			
			redtotal+=ppmred[rowcolumnfinder(x,y)]*deger3;
			greentotal+=ppmgreen[rowcolumnfinder(x,y)]*deger3;
			bluetotal+=ppmblue[rowcolumnfinder(x,y)]*deger3;
			
			redtotal+=ppmred[rowcolumnfinder(x+1,y)]*deger2;
			greentotal+=ppmgreen[rowcolumnfinder(x+1,y)]*deger2;
			bluetotal+=ppmblue[rowcolumnfinder(x+1,y)]*deger2;
			
			redtotal+=ppmred[rowcolumnfinder(x+2,y)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x+2,y)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x+2,y)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x-1,y+1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x-1,y+1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x-1,y+1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y+1)]*deger2;
			greentotal+=ppmgreen[rowcolumnfinder(x,y+1)]*deger2;
			bluetotal+=ppmblue[rowcolumnfinder(x,y+1)]*deger2;
			
			redtotal+=ppmred[rowcolumnfinder(x+1,y+1)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x+1,y+1)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x+1,y+1)]*deger1;
			
			redtotal+=ppmred[rowcolumnfinder(x,y+2)]*deger1;
			greentotal+=ppmgreen[rowcolumnfinder(x,y+2)]*deger1;
			bluetotal+=ppmblue[rowcolumnfinder(x,y+2)]*deger1;

/*controling the red green and blue values so as to make them stable between 0 and max boundaries*/			
			if(redtotal<0)
				redtotal=0;
			if(redtotal> max)
				redtotal=max;
			
			if(greentotal<0)
				greentotal=0;
			if(greentotal> max)
				greentotal=max;
			
			if(bluetotal<0)
				bluetotal=0;
			if(bluetotal> max)
				bluetotal=max;
			
			redtotals[rowcolumnfinder(x,y)]=redtotal;
			greentotals[rowcolumnfinder(x,y)]=greentotal;
			bluetotals[rowcolumnfinder(x,y)]=bluetotal;
		}
	
/*writing output values to output file*/	
	for(i=0;i<row*column;i++)
	{
		fprintf(fileout,"%d %d %d ",redtotals[i],greentotals[i],bluetotals[i]);
	}
}

void canny() 
{
	        printf("canny() function is called\n");
}

void mask() 
{
	        printf("mask() function is called\n");
}
/* ------------------------------------------------------- */
/*function for unexpected conditions*/
void print_usage() {
	printf("\nProgrami yanlis sekilde calistirdiniz, \n\
			lutfen asagidaki siralamalardan birini kullanin:\n\n");
	printf(">  parta <girdi dosyasi> <cikti dosyasi> [b|s|l|m|c]\n");
	printf(">  parta <girdi dosyasi> <cikti dosyasi> -f <maske dosyasi>\n\n");
	exit(1);
}
/* ------------------------------------------------------- */

int main(int argc, char** argv) 
{
	
/*opening the files for reading (input file) and writing (output) file*/
	filein = fopen(argv[1], "r");
	fileout = fopen(argv[2], "w");
	
/* Now it will determine if it's a mask operation  or a built-in function operation. */
	if (argc == 5) 
	{
		if ( (argv[3][0] != '-') || (argv[3][1] != 'f')
				|| (argv[3][2] != '\0') ) print_usage();
		Operationtype = 'f';
		Maskfile = fopen(argv[4], "r");
		mask();         

/* mask read function is called here.*/
	}
	else if (argc == 4) 
	{
		Maskfile = NULL;        

/* mask file won't be used.*/
		if (argv[3][1] != '\0') print_usage();
		switch (argv[3][0]) 
		{
			
/* this switch block determines the type of the built-in operation, and calls its function.*/
			case 'b':       Operationtype = 'b';
					readfile(filein);
					writefile(fileout);
					blur();
					break;
			case 's':       Operationtype = 's';
					readfile(filein);
					writefile(fileout);
					sharpen();
					break;
			case 'm':       Operationtype = 'm';
					readfile(filein);
					writefile(fileout);
					medianfilter();
					break;
			case 'l':       Operationtype = 'l';
					readfile(filein);
					writefile(fileout);
					Log();	
					break;
			case 'c':       Operationtype = 'c';
					canny();
					break;
			default:        printf("unexpected arguments!..\t");
					print_usage();
		}
	}
	else print_usage();
	
	return 0;
	
}
