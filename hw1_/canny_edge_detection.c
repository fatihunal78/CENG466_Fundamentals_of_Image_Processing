/* !!! (mh)  What the heck is this?
 * #pragma ident "%Z%%M% %I% %E%"
 */
/*
**
** 		      Robot Vision Group
** 		Dept. of Artificial Intelligence
** 		    University of Edinburgh
** 
** Authors: Bob Fisher, Dave Croft, A Fitzgibbon
** Date: September 86
** Program: canny.c
** Current Maintainer: andrewfg@ed.ac.uk
**
** Purpose: to apply a simple symmetric canny operator to any size image
**
**
** File Format:
**    Raw PGM (Portable Graymap) from the PBMPLUS package.  To quote from the
**    manpage, "The portable graymap format is a lowest common denominator
**    grayscale file format."  A PGM file as read by this program is in this
**    format:
**      P5
**      <width> <height>
**      255
**      <data as bytestream, row-major order>
**
**    It's trivial to convert it to use other formats -- just change the
**    routines 'fread_header' and 'fwrite_header'
**
** Usage:
**    canny [-d <standard dev>] [-l <thresh>] [-h <thresh>]
**          [-o <file>] [-s <file>]
**          [-p <file> <file>] < input_pic_file > output_pic_file
**
**     -d   followed by a floating point number, this specifies the
**	    standard deviation used during the edge detection phase
**	    - defaults to 1.
**
**     -l   Followed by an integer, this specifies the lower thres-
**	    hold  grey  level  to  be  used  by  the  edge tracker.
**	    Default value is  0;  if  you  are  experimenting  with
**	    threshold values, 20 is a good starting point.
**
**     -h   Followed by an integer, this specifies the upper thres-
**	    hold  grey  level  to  be  used  by  the  edge tracker.
**	    Default value is 255; if  you  are  experimenting  with
**	    threshold values, 35 is a good starting point.
**
**     -s   followed by a file name, this  causes  the  program  to
**	    dump  edge  strength data, in byte format, in the given
**	    file.
**
**     -o   followed by a file name, this  causes  the  program  to
**	    dump  edge  orientation  data,  in  byte format, in the
**	    given file.
**
**     -p   Followed by the names of strength and orientation  data
**	    files  (respectively),  causes  the program to skip the
**	    edge detection phase, and uses the strength and  orien-
**	    tation  data  to  track edges.  This allows the user to
**	    experiment with  different  upper  and  lower  tracking
**	    threshold  values,  without  having  to repeat the time
**	    consuming edge detection phase each time.
**
**
*/

/* Downloaded 28 Nov 1999 from ftp://ftp.dai.ed.ac.uk/pub/vision/src/pgm-canny.c
 * My changes are marked with !!! (mh)
 * Mike Hewett   hewett@cs.utexas.edu
 *
 * To compile: gcc -g -lm -O -Wall canny-86.c -o canny-86
 *
 *   There are many "unused variable" warnings.
 */

#include <math.h>
#include <stdio.h>
/* !!! (mh)  stdlib is needed for malloc() */
#include <stdlib.h>

/* !!! (mh)  gcc doesn't know the register type. */
typedef int REGISTER;



/* scales angles in radians to fit within the 0-255 range of unsigned
   char variables */
#define ORIENT_SCALE 40.0

/* predeclare gaussian function */
double gaussian();

struct header {
  int rows;
  int cols;
};

char Progname[]="canny";


/*            canny_core.c                */

/* core routine for the canny family of programs */


/* hypot can't cope when both it's args are zero, hence this hack.... */
double hypotenuse(x,y)
double x,y;
{
    if (x==0.0 && y==0.0) return(0.0);
    else return(hypot(x,y));
}

/* !!! (mh)  added void return type */
void
canny_core(s,cols,rows,data,derivative_mag,magnitude,orientation)
     double s;            /* standard deviation */
     int cols,rows;            /* picture dimensions */
     unsigned char *data,*derivative_mag,*magnitude,*orientation;
{
  int filter_width;               /* length of 1-D gaussian mask */
  float *gsmooth_x,*gsmooth_y;
  float *derivative_x,*derivative_y;
  int i,j,k,n;            /* counters */
  int t;                /* temp. grad magnitude variable */
  double a,b,c,d,g0;        /* mask generation intermediate vars*/
  double ux,uy;
  double t1,t2;
  double G[20],dG[20],D2G[20];    /*gaussian & derivative filter masks*/
  double gc,gn,gs,gw,ge,gnw,gne,gsw,gse;
  int picsize,jstart,jlimit;
  int ilimit;
  REGISTER jfactor;
  int kfactor1,kfactor2;
  int kfactor;
  REGISTER cindex,nindex,sindex,windex,eindex,nwindex,neindex,swindex,seindex;
  int low=1,high=255;        /* tracker hysteresis parameters */
  int cols_plus,cols_minus;    /*cols+1 and cols-1 respectively*/
  int mag_overflow_count=0;    /* used to measure how oft mag array overflows */
  int mag_underflow_count=0;    /* used to measure how oft mag array underflows */

  picsize=cols*rows;        /* picture area */

  /* calc coeffs for 1-dimensional G, dG/dn and
     Delta-squared G filters */
  for(n=0; n<20; ++n)
    {
      a=gaussian(((double) n),s);
      if (a>0.005 || n<2)
        {
	  b=gaussian((double)n-0.5,s);
	  c=gaussian((double)n+0.5,s);
	  d=gaussian((double)n,s*0.5);

          /* !!! (mh)  changed 'lf' to 'f' */
	  fprintf(stderr,"a,b,c: %f,%f,%f\n",a,b,c);
	  G[n]=(a+b+c)/3/(6.283185*s*s);
	  dG[n]=c-b;
	  D2G[n]=1.6*d-a; /* DOG */
	  fprintf(stderr,"G[%d]: %f\n",n,G[n]);
	  fprintf(stderr,"dG[%d]: %f\n",n,dG[n]);
	  fprintf(stderr,"D2G[%d]: %f\n",n,D2G[n]);
                            
        }
      else break;
    }
  filter_width=n;

  fprintf(stderr,"canny_core: smooth pic\n");
  /* allocate space for gaussian smoothing arrays */
  if ((gsmooth_x=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      fprintf(stderr,"can't alloc gsmooth_x\n");
      exit(0);
    }
  if ((gsmooth_y=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      fprintf(stderr,"can't alloc gsmooth_y\n");
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
	  for(k=1,kfactor1=cindex-cols,
                kfactor2=cindex+cols;
	      k<filter_width;
	      k++,
                kfactor1-=cols,
                kfactor2+=cols)
            {
	      t1+=G[k]*(data[kfactor1]+
			data[kfactor2]);
	      t2+=G[k]*(data[cindex-k]+
			data[cindex+k]);
            }
	  gsmooth_x[cindex]=t1;
	  gsmooth_y[cindex]=t2;
        }
    }
    
  /* allocate space for gradient arrays */
  fprintf(stderr,"canny_core: find grad\n");
  if ((derivative_x=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      fprintf(stderr,"can't alloc x\n");
      exit(0);
    }
  /* produce x and y convolutions with derivative of
     gaussian */

  for (i=filter_width-1;i<ilimit;++i)
    {
      for(jfactor=jstart;
	  jfactor<jlimit;
	  jfactor+=cols)
        {
	  t1=0;
	  cindex=i+jfactor;
	  for (k=1;k<filter_width;++k)
	    t1+=dG[k]*(gsmooth_x[cindex-k]-
                       gsmooth_x[cindex+k]);
	  derivative_x[cindex]=t1;
        }
    }
  free(gsmooth_x);
  if ((derivative_y=(float *)calloc(picsize,sizeof(float)))==(float *)NULL)
    {
      fprintf(stderr,"can't alloc y\n");
      exit(0);
    }

  for (i=n;i<cols-n;++i)
    {
      for(jfactor=jstart;jfactor<jlimit;jfactor+=cols)
        {
	  t2=0;
	  cindex=i+jfactor;
	  for (k=1,kfactor=cols;
	       k<filter_width;
	       k++,kfactor+=cols)
	    t2+=dG[k]*(gsmooth_y[cindex-kfactor]-gsmooth_y[cindex+kfactor]);
	  derivative_y[cindex]=t2;
        }
    }
  free(gsmooth_y);
    
  /* non-maximum suppression (4 cases for orientation of line
     of max slope) */

  fprintf(stderr,"canny_core: non-maximum suppression\n");
  ilimit=cols-filter_width;
  jstart=cols*filter_width;
  jlimit=cols*(rows-filter_width);

  for (i=filter_width;i<ilimit;++i)
    {
      for (jfactor=jstart;
	   jfactor<jlimit;
	   jfactor+=cols)
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
	  t=gc*20.0;
/*fprintf(stderr,"canny_core: i,j=(%d,%d), t=%lf\n",i,jfactor/cols,t);*/
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
		  if((g0=fabs(uy*gc))
                     < fabs(ux*gse+(uy-ux)*gs) ||
                     g0<=fabs(ux*gnw+(uy-ux)*gn))
                    continue;
                }
	      else
                {
		  if((g0=fabs(ux*gc))
                     < fabs(uy*gse+(ux-uy)*ge) ||
                     g0<=fabs(uy*gnw+(ux-uy)*gw))
                    continue;
                }
            }
	  else
            {
	      if(fabs(ux)<fabs(uy))
                {
		  if((g0=fabs(uy*gc))
                     < fabs(ux*gne-(uy+ux)*gn) ||
                     g0<=fabs(ux*gsw-(uy+ux)*gs))
		    continue;
                }
	      else
                {
		  if((g0=fabs(ux*gc))
                     < fabs(uy*gne-(ux+uy)*ge) ||
                     g0<=fabs(uy*gsw-(ux+uy)*gw))
		    continue;
                }
            }
	  /* seems to be a good scale factor */
	  magnitude[cindex]=derivative_mag[cindex];
	  /* pi*40 ~= 128 - direction is (thought
	     of as) a signed byte */
	  orientation[cindex]=atan2(uy, ux)*ORIENT_SCALE;
        }
    } 

  free(derivative_x);
  free(derivative_y);
}

/*            canny_subr.c                */

/* Subroutines used by *canny progs (but not by *canny_j progs) */

/* gaussian function (centred at the origin and ignoring the factor
   of 1/(s*sqrt(2*PI)) ) */
double gaussian(x,s)
double x,s;
{
    return(exp((-x*x)/(2*s*s)));
}

/* follow a connected edge */
/* !!! (mh)  added int return type */
int
follow(i,j,low,cols,rows,data,magnitude,orientation)
int i,j;    /* start point */
int low;    /* lower threshold value */
int cols,rows;    /* picture dimensions */
unsigned char *data;    /* o/p pic array */
unsigned char *magnitude;    /* gradient magnitude array */
unsigned char *orientation;    /* gradient direction array */
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
/*fprintf(stderr,"follow: i,j=%d %d, i_plus_1,i_minus_1=%d %d\n",i,j,i_plus_1,i_minus_1);*/
    if (!data[index])
    {
/*fprintf(stderr,"following %d %d\n",i,j);*/
        /* current point must be added to the list of tracked points */
        data[index]=magnitude[index];
        /* now we can check immediately adjacent points to see if
           they too could be added to the track */
        break_flag=0;
        for (k=i_minus_1;k<=i_plus_1;k++)
        {
            for(l=j_minus_1;l<=j_plus_1;++l)
            {
                kindex=k+l*cols;
                if (!(l==j && k==i) &&
                    magnitude[kindex]>=low/* &&
                    abs(abs(orientation[index]-orientation[kindex])-128)>120*/)
                {
                    if (follow(k,l,low,cols,rows,data,magnitude,orientation))
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


/* !!! (mh)  moved thresholding_tracker() below follow() */

/* track the edges in the magnitude array, starting at points that exceed
   the "high" threshold, and continuing to track as long as point values
   don't duck below the "low" threshold (yes, it's hysteresis...I'm sure
   that hyster means "womb" (as in hysterical), but I don't know what
   it's doing in a common engineering term) */
/* !!! (mh)  added void return type */
void
thresholding_tracker(high,low,cols,rows,data,magnitude,orientation)
int high,low;        /* threshold values */
int cols,rows;        /* picture size */
unsigned char *data;    /* o/p pic array */
unsigned char *magnitude;    /* gradient magnitude array */
unsigned char *orientation;    /* gradient direction array */
{
    int i,j,k;    /* counters */
    int picsize;    /* picture area */

fprintf(stderr,"thresholding_tracker: tracking edges, high=%d, low=%d\n",high,low);
    /* clear data array before tracking */
    picsize=cols*rows;
    for (i=0;i<picsize;++i) data[i]=0;

    /* threshold image with hysteresis: follow from
       each strong edge point */
    for (i=0;i<cols;++i)
    {
        for (j=0;j<rows;++j)
            if (magnitude[i+cols*j]>=high)
                follow(i,j,low,cols,rows,data,magnitude,orientation);
    }
}


int getline_aux (FILE *file, char *buffer, unsigned int n)
{
  int reading = 0;
  while (!reading) {
    if (!fgets (buffer, n, file))
      return 0;
    reading = (buffer [0] != '#');
  }
  return 1;
}

/* !!! (mh)  added void return type */
/* !!! (mh)  moved error() above fread_header() */
void
error(char *msg)
{
  fprintf(stderr, "canny: error in %s\n",msg);
  exit(-1);
}


int fread_header(FILE * fp, struct header * hd)
{
  int maxgray;
  char buf2[3];
  char buf[256];
  
  if (!getline_aux (fp, buf, 256) || (sscanf(buf, "%s", buf2) != 1))
    error("fread_header: first line");
  
  if (strcmp(buf2,"P5") !=0)
    error("fread_header: Not a raw PGM file");

  if (!getline_aux (fp, buf, 256) || (sscanf(buf, "%d %d", &(hd->cols),&(hd->rows)) != 2))
    error("fread_header: couldn't read X Y");

  if (!getline_aux (fp, buf, 256) || (sscanf(buf, "%d", &maxgray) != 1))
    error("fread_header: couldn't read maxgray");

  if (maxgray > 255)
    error("fread_header: maxgray > 255");
  
  return 1;
}

void
fwrite_header(FILE * fp, struct header * hd)
{
  fprintf(fp,"%s\n","P5");
  fprintf(fp,"%d %d\n", hd->cols, hd->rows);
  fprintf(fp,"%d\n",255);
}



/* !!! (mh)  added return type on main() */
/* !!! (mh)  Moved main() to the end of the file to eliminate forward refs */
int
main(argc,argv)
int argc;
char *argv[];
{
  int i,j,k,n;
  double s=1.0;            /* standard deviation of gaussian */
  long picsize;
  struct header hd,shd,ohd;       /* HIPS headers */
  int low=1,high=255;        /* tracker hysteresis parameters */
  char strength_flag=0,orientation_flag=0;    /* output flags */
  char magnitude_file[20],orientation_file[20];    /* i/o file names */
  FILE * sfd, *ofd;                    /*i/o file descriptors*/
  char predigested_ip_flag=0;            /* input flag */
  unsigned char *data;    /* input and output array */
  unsigned char *derivative_mag; /*mag of del G before non-maximum suppression*/
  unsigned char *magnitude,*orientation; /*mag & orient after non-max sppression*/


  /* read args */
  for (i=1;i<argc;i++)
  {
    if (argv[i][0]=='-')
    {
      switch (argv[i][1])
      {
          case 'l': /* track threshold lower limit */
              sscanf(argv[++i],"%d",&low);
              if (low<1 || low>255 || low>high)
              {
                fprintf(stderr,"canny: lower threshold out of range\n");
                exit(0);
              }
              break;
          case 'h': /* track threshold upper limit */
              sscanf(argv[++i],"%d",&high);
              if (high<0 || high>255 || high<low)
              {
                fprintf(stderr,"canny: upper threshold out of range\n");
                exit(0);
              }
              break;
          case 'd': /* arg is s.d. of gaussian */
              s=atof(argv[++i]);
              if (s<0.0)
              {
                fprintf(stderr,"canny: standard deviation out of range\n");
                exit(0);
              }
              break;
          case 's': /* dump strength data to a file */
              if (predigested_ip_flag)
              {
                fprintf(stderr,"canny: -s flag incompatible with -p flag\n");
                exit(0);
              }
              sscanf(argv[++i],"%s",magnitude_file);
              if ((sfd=fopen(magnitude_file,"w"))==0)
              {
                fprintf(stderr,"canny: can't open %s\n",magnitude_file);
                exit(0);
              }
              strength_flag=1;
              break;
          case 'o': /* dump orientation data to a file */
              if (predigested_ip_flag)
              {
                fprintf(stderr,"canny: -o flag incompatible with -p flag\n");
                exit(0);
              }
              sscanf(argv[++i],"%s",orientation_file);
              if ((ofd=fopen(orientation_file,"w"))==0)
              {
                fprintf(stderr,"canny: can't open %s\n",orientation_file);
                exit(0);
              }
              orientation_flag=1;
              break;
          case 'p': /* get i/p from strength & orient files */
              if (strength_flag || orientation_flag)
              {
                fprintf(stderr,"canny: -p flag incompatible with -s and -o flags\n");
                exit(0);
              }
              predigested_ip_flag=1;
              sscanf(argv[++i],"%s",magnitude_file);
              sscanf(argv[++i],"%s",orientation_file);
              if ((sfd=fopen(magnitude_file,"r"))==0)
              {
                fprintf(stderr,"canny: can't open %s\n",magnitude_file);
                exit(0);
              }
              if ((ofd=fopen(orientation_file,"r"))==0)
              {
                fprintf(stderr,"canny: can't open %s\n",orientation_file);
                exit(0);
              }
              break;
          default:
              fprintf(stderr,"canny: unknown option -%c\n",argv[i][1]);
              exit(1);
              break;
      }
    }
  }

  if (predigested_ip_flag)
  {
    fread_header(sfd,&shd);
    fread_header(ofd,&ohd);
    if (shd.cols!=ohd.cols || shd.rows!=ohd.rows)
    {
      fprintf(stderr,"canny: files must be the same size\n");
      exit(0);
    }
    hd=shd;
  }
  else
    fread_header(stdin, &hd);
  picsize=hd.cols*hd.rows;

  fwrite_header(stdout, &hd);
  if (strength_flag)
  {
    shd=hd;
    fwrite_header(sfd,&shd);
  }
  if (orientation_flag)
  {
    ohd=hd;
    fwrite_header(ofd,&ohd);
  }

  /* allocate i/o array */
  if ((data=(unsigned char *)malloc(picsize))==(unsigned char *)NULL)
  {
    fprintf(stderr,"canny: can't alloc data\n");
    exit(0);
  }
  if ((derivative_mag=(unsigned char *)calloc(picsize,1))==
      (unsigned char *)NULL)
  {
    fprintf(stderr,"canny: can't alloc derivative_mag\n");
    exit(0);
  }
  if ((magnitude=(unsigned char *)calloc(picsize,1))
      ==(unsigned char *)NULL)
  {
    fprintf(stderr,"canny: can't alloc magnitude\n");
    exit(0);
  }
  if ((orientation=(unsigned char *)calloc(picsize,1))
      ==(unsigned char *)NULL)
  {
    fprintf(stderr,"canny: can't alloc orientation\n");
    exit(0);
  }

  if (predigested_ip_flag)
  {    /* read in strength and direction data, then go straight
          on to edge tracking */
    fprintf(stderr,"canny: reading predigested i/p data\n");
    if (fread(magnitude,picsize,1,sfd)!=1)
    {
      fprintf(stderr,"canny: incomplete strength file\n");
      exit(0);
    }
    if (fread(orientation,picsize,1,ofd)!=1)
    {
      fprintf(stderr,"canny: incomplete orientation file\n");
      exit(0);
    }
  }
  else
  {    /* no predigested i/p - `fraid we've got to calculate
          strength and direction data from scratch */
    /* read a frame from stdin */
    fread(data,picsize,1,stdin);
	  
    /* call canny core routines - these perform a gaussian
       smoothing, calculate a gradient array and suppress
       non- maximal points in this array */
    canny_core(s,hd.cols,hd.rows,data,derivative_mag,magnitude,orientation);
  }
    
  /* !!! (mh)  Changed write() to printf() */
  /* optionally write out strength and orientation info */
  /*
  if (strength_flag) write(sfd,magnitude,picsize);
  if (orientation_flag) write(ofd,orientation,picsize);
  */
  if (strength_flag) printf("Source file = %ld, magnitude = %s, picsize = %ld\n",
                            (long)(sfd), magnitude, picsize);
  if (orientation_flag) printf("Output file = %ld, orientation = %s, picsize = %ld\n",
                               (long)(ofd), orientation, picsize);


  fprintf(stderr,"canny: about to track edges\n");
  /* track edges */
  thresholding_tracker(high,low,hd.cols,hd.rows,data,magnitude,orientation);

  /* write tracked edges to standard output */
  fwrite(data,picsize,1,stdout);

  free(data);
  free(magnitude);
  free(orientation);

  return 0;
}
