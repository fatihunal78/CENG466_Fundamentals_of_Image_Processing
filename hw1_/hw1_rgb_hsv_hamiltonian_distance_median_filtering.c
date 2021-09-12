/********************************/
/*	Fatih Unal, Faruk Onen	*/
/*	(e1250885)  (e1298074)	*/
/*	    Ceng466 - HW1	*/
/*		2003		*/
/********************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PARTA		1
#define PARTB		2
#define PARTC		3
#define RGB		4
#define HSV		5
#define INFINITY	6

int part, model, order; /*input olarak girilen argumanlar */
int x_1, y_1, max1; /* 1. resmin buyuklugu ve en buyuk piksel degeri */
int x_2, y_2, max2; /* 2. resmin buyuklugu ve en buyuk piksel degeri */

/* 1. ve 2. input resimleri, output dosyalari icin file pointerlari */
FILE *girdi1, *girdi2, *cikti1,
	*cikti2, *result_text;

void convert_pixel_to_HSV(FILE* f1, FILE* f2)
{
/*fonksiyon RGB color modelindeki her bir pixeli HSV color modeline ceviriyor */
	double R,G,B,minRGB,H,S,V,fark;
	int r,g,b;
	fscanf(f1, "%d %d %d", &r, &g, &b);
/*dosyadan red green blue input argumanlarini okuyor*/
	/*printf("%d %d %d\n", r, g, b);*/
	R = (double) r;
	G = (double) g;
	B = (double) b;
/* calculating value...*/
	if (R>G) {	
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
		if (R>B) {
			minRGB = B;
			V = G;
		}
		else { /* B>=R */
			minRGB = R;
			if (G>B) V = G;
			else V = B; /* B>=G */
		}
	}			
	fark = V - minRGB;

/* calculating saturation... */
	
	if (V == 0.0) 
		S = 0.0;
	else
		S = fark / V;

/* calculating hue... */

	if (S == 0.0)
		H = 0.0;
	else {
		if (R == V)
			H = 60.0 * (G-B) / fark;
		else {
			if (G == V)
				H=120.0+60.0*(B-R)/fark;
			else 
				H=240.0+60.0*(R-G)/fark;
		}
	}

	if (H<0.0)
		H = H + 360.0;

	fprintf(f2,"%.3f %.3f %.3f ",H,S,V/255.0);
/*	printf("%.3f %.3f %.3f\n",H,S,V/255.0);*/
	
}

void convert_HSV_to_RGB(FILE* f1, FILE* f2)
{
/*bu fonksiyon HSV color modelindeki herbir pixeli RGB color modeline ceviriyor.*/
	double R,G,B,temp_h,H,S,V,frac_h;
	float h,s,v;
	double p,q,t;
	int i;

	fscanf(f1, "%f %f %f", &h, &s, &v);
/*dosyadan hue saturation ve value degerlerini okuyor.*/	
	H = (double) h;
	S = (double) s;
	V = (double) v;

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
	
/*Calculating red green and blue...*/
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
	
	R=(int) (R*255+0.5);
	G=(int) (G*255+0.5);
	B=(int) (B*255+0.5);
/*RGB degerlerini output doyasina yaziyor.*/		
	fprintf(f2,"%.0f %.0f %.0f ", R,G,B);
	
}


void convert_file_to_HSV(FILE* f1, FILE* f2)
/* resim dosyasini RGBden HSVye cevirir.ilk pointer inputtan okunan dosyanin,ikinci pointer da gecici olarak yaratilan ve icinde RGB degerlerinin HSV ye cevirilmis halinibulunduran dosyanin pointeridir.*/
{
	int en, boy, max, i;
	char ch1=0;

	rewind(f1);
	
/*P3 ve # ile baslayan satirlari atlamak icin asagidaki looplari kullaniyoruz.*/	
	while((ch1=fgetc(f1))!='\n') {}
	ch1=fgetc(f1);
	if (ch1=='#')
	{
		do
		{
			ch1=fgetc(f1);
		}
		while(ch1!='\n');
	}
	else ungetc(ch1,f1);
/*en boy ve max degerlerini okur ve gecici dosyaya yazar.*/	
	fscanf(f1, "%d %d %d", &en, &boy, &max); 
	fprintf(f2, "%d %d\n%d\n", en, boy, max); 
/*	printf("%d %d %d\n", en, boy, max);*/
/*birer birer tum pixellerdeki RGB degerlerini HSV ye cevirir.*/	
	
	for(i=0; i<en*boy; i++)
	{
		convert_pixel_to_HSV(f1, f2);
	}
}

void convert_file_to_RGB(FILE* f1, FILE* f2)
{
/* icinde HSV color modelinde pixeller bulunduran dosyayi acar,en boy max degerlerini okur ve birer birer pixelleri ele alip bunlari RGB color modeline cevirir.*/
	int en, boy, max, i;
	fscanf(f1, "%d %d %d", &en, &boy, &max);
	fprintf(f2, "%d %d\n%d\n", en, boy, max);
	for(i=0; i<en*boy; i++)
	{
		convert_HSV_to_RGB(f1, f2);
	}
}

void distancefinder(FILE* f1 ,FILE* f2)
{
/*Part A da istenen hamiltonian distance euclidean distance ve p=infinity icin distance i bulur.Bu fonksiyon iki gecici dosyayi acar,ilgili uzakligi hesaplar ve sonucu output dosyasina yazar.*/
	int en1=0,boy1=0,max_1=0,en2=0,boy2=0,max_2=0;
	double hamilton=0,euclidian=0,mydistance=0;
	int i,j,buyuk=0,enbuyuk=0;

	double hsv1, hsv2;
	int rgb1,rgb2;
	
	int hangienbuyuk, hangiboybuyuk;

/*gecici dosyalardan en boy ve max degerlerini okuyor*/
/*
	fscanf(f1, "%d %d %d", &en1, &boy1, &max_1);
	fscanf(f2, "%d %d %d", &en2, &boy2, &max_2);
	*/
	
	en1 = x_1; en2 = x_2; boy1 = y_1; boy2 = y_2; 
	
	printf("%d %d %d\n", en1, boy1, max_1);
	printf("%d %d %d\n", en2, boy2, max_2);
	
	en1 *= 3;
	en2 *= 3;

	if (en1 > en2) hangienbuyuk=1;
	else hangienbuyuk=2;

	if (boy1 > boy2) hangiboybuyuk=1;
	else hangiboybuyuk=2;
	
/*ppm dosyalari arasindaki buyukluk farkina gore ayarlama yapiyor.*/

	/* buyukluk farkiyla ilgili ekleme yapilacak  */
if (model==RGB) {
	for(i=0; i < (hangiboybuyuk==1?boy1:boy2); i++) 
	{
		for(j=0; j < (hangienbuyuk==1?en1:en2); j++)
		{
		/*	printf("j: %d\ten1: %d\tboy1: %d\ten2: %d\tboy2: %d"
					, j, en1, boy1, en2, boy2); */
			if (j >= (hangienbuyuk==1?en2:en1)) {
				printf("if'e girdi\n");
				(hangienbuyuk==1?rgb2:rgb1) = 255;
				fscanf((hangienbuyuk==1?f1:f2),"%d",&(hangienbuyuk==1?rgb1:rgb2));
			}
			else if (i > (hangiboybuyuk==1?boy2:boy1))
			{
				printf("else if'e girdi\n");
				(hangiboybuyuk==1?rgb2:rgb1) = 255;
				fscanf((hangiboybuyuk==1?f1:f2), "%d", &(hangiboybuyuk==1?rgb1:rgb2));
			}
			else {
				/*printf("else okuyor\n");*/
				fscanf(f1, "%d", &rgb1);
				fscanf(f2, "%d", &rgb2);
				printf("rgb1: %d\trgb2: %d\n", rgb1, rgb2);
			}
			


			
			if (order==1)
			{
				hamilton += fabs(rgb1-rgb2);
			/*	printf("%d\n",abs(rgb1-rgb2)); */
				printf("%.2f\n",hamilton);
			}
			if (order==2)
			{
				euclidian += pow( fabs(rgb1 - rgb2), 2);
				printf("%.2f\n",euclidian);
			}
			if (order==INFINITY)
			{
				buyuk = fabs(rgb1-rgb2);
				if(buyuk>enbuyuk)
					enbuyuk=buyuk;
				mydistance = enbuyuk;
				printf("%.2f\n", mydistance);
			}
		}
	}
	
	if (order==1)
	{
		fprintf(result_text,"%.2f ",hamilton);
	}
	if (order==2)
	{
		euclidian = sqrt(euclidian);
		fprintf(result_text,"%.2f",euclidian);
	}
	if (order==INFINITY)
	{
		fprintf(result_text, "%.2f",mydistance);
	}
}

if (model==HSV)
{
	for(i=0; i < (hangiboybuyuk==1?boy1:boy2); i++)
	{
		for(j=0; j < (hangienbuyuk==1?en1:en2); j++)
		{
			if (j >= (hangienbuyuk==1?en2:en1)) {
				(hangienbuyuk==1?hsv2:hsv1) = 255;
				fscanf((hangienbuyuk==1?f1:f2),"%lf",&(hangienbuyuk==1?hsv1:hsv2));
			}
			else if (i > (hangiboybuyuk==1?boy2:boy1))
			{
				(hangiboybuyuk==1?hsv2:hsv1) = 255;
				fscanf((hangiboybuyuk==1?f1:f2), "%lf", &(hangiboybuyuk==1?hsv1:hsv2));
			}
			else {
				fscanf(f1, "%lf", &hsv1);
				fscanf(f2, "%lf", &hsv2);
			}
			
			
			
			
			if (order==1)
			{
				hamilton += fabs(hsv1-hsv2);
				printf("%.2f\n",hamilton);
			}
			if (order==2)
			{
				euclidian += pow( fabs(hsv1 - hsv2), 2);
				printf("%.2f\n",euclidian);
			}
			if (order==INFINITY)
			{
				buyuk = fabs(hsv1-hsv2);
				if(buyuk>enbuyuk)
					enbuyuk=buyuk;
				mydistance = enbuyuk;
				printf("%.2f\n", mydistance);
			}
		}
	}
	
	if (order==1)
	{
		fprintf(result_text,"%.2f ",hamilton);
	}
	if (order==2)
	{
		euclidian = sqrt(euclidian);
		fprintf(result_text,"%.2f\n",euclidian);
	}
	if (order==INFINITY)
	{
		fprintf(result_text, "%.2f\n",mydistance);
	}
	
}

/*	
	if (order == 1)
	{
*hamiltonian distance i formule gore hesaplar,sonucu output dosyasina yazar.*

		
		for(i=0; i < c ; i++)
		{
			fscanf(f1, "%lf %lf %lf", &h1, &s1, &v1);
			fscanf(f2, "%lf %lf %lf", &h2, &s2, &v2);
			printf("%lf %lf %lf\n", h1, s1, v1);
			printf("%lf %lf %lf\n", h2, s2, v2);
			hamilton += fabs(v1-v2);
			printf("%.2f\n",hamilton);
		}
		fprintf(result_text,"%.2f ",hamilton);
	}

	if (order == 2)
	{
*euclidean distance i formule gore hesaplar,sonucu output dosyasina yazar.*
		for(i=0; i < c ; i++)
		{
			fscanf(f1, "%lf %lf %lf", &h1, &s1, &v1);
			fscanf(f2, "%lf %lf %lf", &h2, &s2, &v2);
			printf("%lf %lf %lf\n", h1, s1, v1);
			printf("%lf %lf %lf\n", h2, s2, v2);
			euclidian += pow( fabs(v1 - v2), 2);
			printf("%.2f\n",euclidian);
		}
		sqrt(euclidian);
		fprintf(result_text,"%.2f\n",euclidian);
	}

	if (order == INFINITY)
	{
*order p=infinity icin distance i hesaplar.burda uzaklik,en buyuk fark degeridir.*
		double buyuk=0,enbuyuk=0;
		for(i=0; i < c ; i++)
		{
			fscanf(f1, "%lf %lf %lf", &h1, &s1, &v1);
			fscanf(f2, "%lf %lf %lf", &h2, &s2, &v2);
			printf("%lf %lf %lf\n", h1, s1, v1);
			printf("%lf %lf %lf\n", h2, s2, v2);
			buyuk=fabs(v1-v2);
			if(buyuk>enbuyuk)
				enbuyuk=buyuk;
			mydistance=enbuyuk;
			printf("%.2f\n", mydistance);
		}
		fprintf(result_text, "%.2f\n",mydistance);
	}

*/
}

void do_part_a()	/*PARTA yi yapar*/
{
	FILE *hsv1, *hsv2;	/*gecici dosyalar icin pointerlar*/

	hsv1=fopen("dosya1", "w");	/*gecici dosya 1 i acar ve dosyaya yazar*/
	hsv2=fopen("dosya2", "w");	/*gecici dosya 2 yi acar ve dosyaya yazar*/
	convert_file_to_HSV(girdi1,hsv1);	/*1. input dosyasini HSVye cevirir*/
	convert_file_to_HSV(girdi2,hsv2);	/*2. input dosyasini HSVye cevirir*/
	fclose(hsv1);
	fclose(hsv2);
	hsv1=fopen("dosya1","r");	
	hsv2=fopen("dosya2","r");	

if (model==HSV)
{
	distancefinder(hsv1,hsv2);	/*bu iki gecici dosya arasinda sorulan uzakligi hesaplar*/
}
if (model==RGB)
{
	distancefinder(girdi1, girdi2);
}
/*	if(!(remove("dosya1") || remove("dosya2") || fclose(hsv1) || fclose(hsv2)))
		return;	
*/
}


void RGB_kanallarina_ayir(FILE* fp, int*R, int*G, int*B)
{
	int i;
	for(i=0; i<x_1*y_1; i++)
		fscanf(fp, " %d %d %d", &R[i], &G[i], &B[i]);
	return;
}

void V_kanalini_ayir(FILE* fp, double*V)
{
	double gereksiz1, gereksiz2;
	int i;
	for (i=0; i<x_1*y_1; i++)
		fscanf(fp, "%lf %lf %lf", &gereksiz1, &gereksiz2, &V[i]);
}

int yuvarla(double x) { return (int) (x + 0.49); }

void RGBhistogram_esitle(int* eski, int* yeni)
	/* eski R/G/B degerlerinden histogram cikarir
	 * *          * ve histogrami esitler */
{
	int hist[max1], cum_hist[max1], i, esit_hist[max1];
	double carpan;
	
	for(i=0; i < max1; i++)
		hist[i]=cum_hist[i]=0;
	for(i=0; i < x_1*y_1; i++)
		hist[eski[i]]++;
	cum_hist[0] = hist[0];
	for(i=1; i < max1; i++)
		cum_hist[i] = cum_hist[i-1] + hist[i];
	carpan = ((double)(max1 - 1)) / ((double) x_1 * y_1);
	for(i=0; i < max1; i++)
		esit_hist[i] = yuvarla(carpan * cum_hist[i]);
	for(i=0; i < x_1 * y_1; i++)
		yeni[i] = esit_hist[eski[i]];
	
	return;
}


void do_part_b()
{
/*      FILE* hsv = fopen("466_hw1_gecici_dosya", "w");*/
	FILE* hsv = tmpfile();
	int     i_buyukluk = ((sizeof(int)) * x_1 * y_1),
	d_buyukluk = ((sizeof(double)) * x_1 * y_1);
	int     *R_kanali = (int *) malloc(i_buyukluk),
	*G_kanali = (int *) malloc(i_buyukluk),
	*B_kanali = (int *) malloc(i_buyukluk),
	*yeni_R = (int *) malloc(i_buyukluk),
	*yeni_G = (int *) malloc(i_buyukluk),
	*yeni_B = (int *) malloc(i_buyukluk);
	double  *V_kanali = (double *) malloc(d_buyukluk),
	*yeni_V = (double *) malloc(d_buyukluk); 
	
	RGB_kanallarina_ayir(girdi1, R_kanali, G_kanali, B_kanali);
	convert_file_to_HSV(girdi1, hsv);
	V_kanalini_ayir(hsv, V_kanali);
	RGBhistogram_esitle(R_kanali, yeni_R);
	RGBhistogram_esitle(G_kanali, yeni_G);
	RGBhistogram_esitle(B_kanali, yeni_B);
	/*      Vhistogram_esitle(V_kanali, yeni_V);
	        RGB_birlestir(R_kanali, G_kanali, B_kanali, cikti1);
	        V_birlestir(hsv, V_kanali, cikti2);
	*/
	printf("b bolumu calisti...\n");
}

void do_part_c()
{

	printf("c bolumu calisti...\n");
}

int main(int argc, char *argv[])
{
	 char ch1=0;
/*1. arguman her zaman executable dosyanin kendisidir*/
/*2. argumanin PARTA olup olmadigina bakar,eger PARTA ise 3. argumanin RGB veya HSV ye esitligini inceler son olarak 4. argumanin 1 2 yada infinity e esitligini kontrol eder.Gerekli deger atamalarini yapar.(model,part ve order)*/
	if (!(strcmp(argv[1], "PARTA"))) 
	{
		printf("PARTA oldugunu anladi!\n");
		part = PARTA;
		
		if (!(strcmp(argv[2], "RGB")))
			model = RGB;
		if (!(strcmp(argv[2], "HSV")))
			model = HSV;
		if (!(strcmp(argv[3], "1")))
			order = 1;
		if (!(strcmp(argv[3], "2")))
			order = 2;
		if (!(strcmp(argv[3], "infinity")))
			order = INFINITY;
/*5. ve 6. argumanlarin gosterdigi dosyalari acar dosyalari okuma izni verir ve bu dosyalara pointerlar doner.7. argumanin gosterdigi dosyayi acar ve dosyaya yazma izni verir ve bu dosyaya pointer dondurur.*/		
		girdi1 = fopen(argv[4],"r");
		girdi2 = fopen(argv[5],"r");
		result_text=fopen(argv[6],"w");
	}

/*2. argumanin PARTB ye esitligine bakar,esitse 3. argumanin gosterdigi dosyadan okuma izni verir ona pointer doner,4. ve 5. argumanlarin gosterdigi dosylara yazma izni verir ve bu dosyalara ilgili pointerlari dondurur.*/	
	if (!(strcmp(argv[1], "PARTB"))) 
	{
		printf("PARTB oldugunu anladi!\n");
		part = PARTB;
		girdi1 = fopen( argv[2], "r" );
		cikti1 = fopen( argv[3], "w" );
		cikti2 = fopen( argv[4], "w" );
	}
	
/*2. argumanin PARTC ye esitligini denetler,esitlik durumunda 3. argumana bakip order i ilgili degere esitler.(1 2 yada infinity) .4. argumanin gosterdigi input dosyasini acip ona pointer dondurur.5.ve 6. argumanlarin gosterdigi dosyalari acar,pointerlarini doner ve dosyalara yazma izni verir*/
	if (!(strcmp(argv[1], "PARTC"))) 
	{
		printf("PARTC oldugunu anladi!\n");
		part = PARTC;
		
		if (!(strcmp(argv[2], "1"))) 
			order = 1;
		if (!(strcmp(argv[2], "2")))
			order = 2;
		if (!(strcmp(argv[2], "infinity")))
			order = INFINITY;
		
		girdi1 = fopen( argv[3], "r" );
		cikti1 = fopen( argv[4], "w" );
		result_text=fopen(argv[5],"w");
	}
	
	 
	 while((ch1=fgetc(girdi1))!='\n') {}
	 ch1=fgetc(girdi1);
	 if (ch1=='#')
	 {
		 do
		 {
			 ch1=fgetc(girdi1);
		 }
		 while(ch1!='\n');
	 }
	 fscanf(girdi1, "%d %d %d", &x_1, &y_1, &max1);
/*	 if (part==PARTA) rewind(girdi1); */
	 
	 if (girdi2)
	 {
		 while((ch1=fgetc(girdi2))!='\n') {}
		 ch1=fgetc(girdi2);
		 if (ch1=='#')
		 {
			 do
			 {
				 ch1=fgetc(girdi2);
			 }
			 while(ch1!='\n');
		 }
		 fscanf(girdi2, "%d %d %d", &x_2, &y_2, &max2);
/*		 if (part==PARTA) rewind(girdi2); */
	 }
	
/*part variable inin degerine gore A B yada C kismini yapar.Ve dosyalarin pointerlarini serbest birakir.*/
	switch (part)
	{
		case PARTA:	do_part_a(); 
				fclose(girdi1);
				fclose(girdi2);
				fclose(result_text);
				break;
				
		case PARTB:	do_part_b(); 
				fclose(girdi1);
				fclose(cikti1);
				fclose(cikti2);
				break;
				
		case PARTC:	do_part_c(); 
				fclose(girdi1);
				fclose(cikti1);
				fclose(result_text);
				break;
				
		default:	printf("farkli birsey deniyorum!...\n");
	}


	return 0;

}
