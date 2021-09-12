/*
 * 	   Ceng466 - HW 2
 * 	      (Part A)
 * 	Fatih Unal, Faruk Onen
 * 	 1250885     1298074
 * 	 
 */


#include <stdlib.h>
#include <stdio.h>

/* variables for command line arguments	*/
FILE* Infile;		/* Input file 	*/
FILE* Outfile;		/* Output file 	*/
FILE* Maskfile;		/* Mask file	*/
char Operationtype;	/* one of blur (b), sharpen (s), LoG (l),
			   median filter (m), canny (c) operations */

/* ------------------------------------------------------- */
  
typedef struct {
	int height, width;	/* height and width of the window */
	float ** w;		/* array holding the pixels */
} WIN;

typedef struct {
	int h, w;
	int ** p;
} BUF;

/*
void convolve(BUF bufi, BUF bufo, WIN win) {
	int ch, i, j, av;
	int fh, fw, maxv;
	fscanf(fi, "P3\n");
	ch = fgetc(fi);
	if (ch=='#')
		while (ch != '\n')
			ch = fgetc(fi);
	
	for(i=1)
}
*/


/* ------------------------------------------------------- */
/* The following 5 functions are for the 5 built-in functions
 * of the program (that are Blur, Sharpen, Median filtering,
 * LoG and Canny). One of these is called according to the
 * operation type. */

void blur() {
	int i, j, w, h, maxv;
	char ch;
	int **ibuf, **obuf;
		
	/* reading the header of the input file */
	fscanf(Infile, "P3\n");
	ch = fgetc(Infile);
	if (ch=='#')
		while (ch != '\n')
			ch = fgetc(Infile);
	fscanf(Infile, "%d %d\n%d\n", &h, &w, &maxv);
	w *= 3;

	/* memory for buffers */
	ibuf = (int **) malloc(sizeof(int*)*h);
	obuf = (int **) malloc(sizeof(int*)*h);
	for(i = 0; i < h; i++) {
		ibuf[i] = (int *) malloc(sizeof(int)*w);
		obuf[i] = (int *) malloc(sizeof(int)*w);
	}

	/* reading input file to input buffer */
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++)
			fscanf(Infile, "%d ", &(ibuf[i][j]));

	/* applying the 3x3 blur window on input buffer */
	for (i = 1; i < h-1; i++)
		for (j = 3; j < w-1; j += 3) {
			obuf[i][j] = ibuf[i-1][j-3] + ibuf[i-1][j] + ibuf[i-1][j+3] 
				+ ibuf[i][j-3] + ibuf[i][j] + ibuf[i][j+3] 
				+ ibuf[i+1][j-3] + ibuf[i+1][j] + ibuf[i+1][j+3];
			obuf[i][j] /= 9;
			if (obuf[i][j] < 0) obuf[i][j] = 0;
			if (obuf[i][j] > maxv) obuf[i][j] = maxv;
		}

	for (i = 0; i < w; i++)
		obuf[0][i] = ibuf[0][i];
	for (i = 0; i < w; i++)
		obuf[h-1][i] = ibuf[h-1][i];
	for (i = 0; i < h; i++) {
		obuf[i][0] = ibuf[i][0];
		obuf[i][1] = ibuf[i][1];
		obuf[i][2] = ibuf[i][2];
	}
	for (i = 0; i < h; i++) {
		obuf[i][w-1] = ibuf[i][w-1];
		obuf[i][w-2] = ibuf[i][w-2];
		obuf[i][w-3] = ibuf[i][w-3];
	}
	
	/* Writing the output buffer to output file */
	fprintf(Outfile, "P3\n# Faruk\n%d %d\n%d\n", h, w/3, maxv);
	for (i = 0; i < h; i++)
		for (j = 0; j < w; j++) {
			if ( (i == h-1) && (j == w-1) ) 
				fprintf(Outfile, "%d", obuf[i][j]);
			else fprintf(Outfile, "%d\n", obuf[i][j]);
		}

	printf("-------------");
	printf("girdi:\n%d %d %d\n%d %d %d\n%d %d %d\n", ibuf[0][0], ibuf[0][1], ibuf[0][2], 
			ibuf[1][0], ibuf[1][1], ibuf[1][2], ibuf[2][0], ibuf[2][1], ibuf[2][2]);
	printf("cikti:\n%d %d %d\n%d %d %d\n%d %d %d\n", obuf[0][0], obuf[0][1], obuf[0][2], 
			obuf[1][0], obuf[1][1], obuf[1][2], obuf[2][0], obuf[2][1], obuf[2][2]);
}
/* ------------------------------------------------------- */

void sharpen() {
	printf("sharpen() function is called\n");
}
/* ------------------------------------------------------- */

void medfilter() {
	printf("medfilter() function is called\n");
}
/* ------------------------------------------------------- */

void Log() {
	printf("Log() function is called\n");
}
/* ------------------------------------------------------- */

void canny() {
	printf("canny() function is called\n");
}
/* ------------------------------------------------------- */

void mask() {
	printf("mask() function is called\n");
}
/* ------------------------------------------------------- */

void print_usage() {
	printf("\nProgrami yanlis sekilde calistirdiniz, \n\
lutfen asagidaki siralamalardan birini kullanin:\n\n");
	printf(">  parta <girdi dosyasi> <cikti dosyasi> [b|s|l|m|c]\n");
	printf(">  parta <girdi dosyasi> <cikti dosyasi> -f <maske dosyasi>\n\n");
	exit(1);
}
/* ------------------------------------------------------- */

int main(int argc, char** argv) {
	Infile = fopen(argv[1], "r");
	Outfile = fopen(argv[2], "w");

	/* Now it will determine if it's a mask operation
	 * or a built-in operation. */
	if (argc == 5) {
		if ( (argv[3][0] != '-') || (argv[3][1] != 'f')
		  || (argv[3][2] != '\0') ) print_usage();
		Operationtype = 'f';
		Maskfile = fopen(argv[4], "r");
		mask();		/* mask read function is called here.*/
	}
	else if (argc == 4) {
		Maskfile = NULL;	/* mask file won't be used.*/
		if (argv[3][1] != '\0') {
			print_usage();
			exit(1);
		}
		switch (argv[3][0]) {	
			/* this switch block determines the type of the 
			 * built-in operation, and calls its function.*/
			case 'b':	Operationtype = 'b'; 
					blur();
					break;
			case 's':	Operationtype = 's'; 
					sharpen();
					break;
			case 'm':	Operationtype = 'm'; 
					medfilter();
					break;
			case 'l':	Operationtype = 'l'; 
					Log();
					break;
			case 'c':	Operationtype = 'c'; 
					canny();
					break;
			default: 	printf("switch icinde hata var!..\t");
					print_usage();
		}
	}
	else print_usage();
	
	return 0;
}
