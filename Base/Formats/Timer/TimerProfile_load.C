#include <stdio.h>
#include <assert.h>

#include "TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "convert_endian.h"

/******************************************************************************/
/* these two functions are used by load and unload functions. */
/* so need to declare them here                               */ 

//  Writes out compressed data

static unsigned short int * packed_buf = NULL;
static unsigned packed_buf_sz = 0;

int fcompwrite (unsigned nvals, const float * vals, FILE * fptr)
{
  if (!vals) {
    cerr << "fcompwrite: invalid vals" << endl;
    return -1;
  }

  if (packed_buf_sz < nvals)  {
    if (packed_buf) delete [] packed_buf; packed_buf = NULL;
    packed_buf = new unsigned short [nvals]; assert (packed_buf != NULL);
    packed_buf_sz = nvals;
  }

  assert (sizeof (unsigned short) == 2);

  float two16 = (1<<16) - 1;

  float xmin,xmax;
  xmin = xmax = vals[0];

  unsigned k=0;
  for (k=0; k<nvals; k++) {
    if (xmin > vals[k]) xmin = vals[k];
    if (xmax < vals[k]) xmax = vals[k];
  }

  float scale;
  if (xmin != xmax)
    scale = two16 / (xmax - xmin);
  else
    scale = 1.0;

  if (scale == 0.0 || isnan(scale)) {
    cerr << "fcompwrite: XMIN=" << xmin << " XMAX=" << xmax 
	 << " SCALE=" << scale << endl;
    return -1;
  }

  float offset = xmin;

  for (k=0; k<nvals; k++)
    packed_buf[k]= (unsigned short)((vals[k] - offset) * scale);

  ToBigEndian (scale);
  ToBigEndian (offset);
  N_ToBigEndian (nvals, packed_buf);

  fwrite (&scale,  sizeof(scale),  1,fptr);
  fwrite (&offset, sizeof(offset), 1,fptr);
  fwrite (packed_buf,  nvals*sizeof(unsigned short), 1,fptr);

  if (ferror(fptr))  {
    perror ("fcompwrite:");
    return -1;
  }

  return 0;
}


/*****************************************************************************/
// reads in compressed data
int fcompread (unsigned nvals, float * vals, FILE * fptr, bool big_endian)
{
  if (packed_buf_sz < nvals)  {
    if (packed_buf) delete [] packed_buf; packed_buf = NULL;
    packed_buf = new unsigned short [nvals]; assert (packed_buf != NULL);
    packed_buf_sz = nvals;
  }

  float offset,scale;
  if (fread(&scale,sizeof(float),1,fptr) < 1)  {
    perror ("fcompread: fail fread scale");
    return -1;
  }
  if (fread(&offset,sizeof(float),1,fptr) < 1)  {
    perror ("fcompread: fail fread offset");
    return -1;
  }
  if (fread(packed_buf,sizeof(unsigned short),nvals,fptr) < nvals)  {
    perror ("fcompread: fail fread nvals");
    fprintf(stderr, "could not read %d bins from archive\n", nvals);
    return -1;
  }
  if (big_endian) {
    FromBigEndian (scale);
    FromBigEndian (offset);
    N_FromBigEndian (nvals, packed_buf);
  }
  else {
    FromLittleEndian   (scale);
    FromLittleEndian   (offset);
    N_FromLittleEndian (nvals, packed_buf);
  }

  if (scale==0 || isnan(scale)) {
    fprintf(stderr, "fcompread error - scale==%f\n", scale);
    return(-1);
  }

  if(isnan(scale)) return(-1);

  for (unsigned k=0;k<nvals;k++)
    vals[k]= (((float) packed_buf[k]) / scale) + offset ;

  if (ferror(fptr))  {
    perror ("fcompread:");
    return -1;
  }

  return 0;
}
 
/*!
  \pre The Profile must have been resized before calling this function
*/
void Pulsar::TimerProfile_load (FILE* fptr, Profile* profile, 
				bool big_endian)
{
  float centrefreq;      /* centre frequency of profile MHz */
  float wt;              /* weight of profile               */
  int   poln;	         /* 0=I, 1=LL, 2=RR, 3=LR, 4=RL, 5=Q, 6=U, 7=V */
  int   nbin;

  //Read in the centre frequency
  if (fread(&centrefreq,sizeof(centrefreq),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread centrefreq");
  //Number of bins
  if (fread(&nbin,sizeof(nbin),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread nbin");
  // Poln
  if (fread(&poln,sizeof(poln),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread poln");
  // Weight
  if (fread(&wt,sizeof(wt),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_load", "fread wt");
 
  if (big_endian) {
    if (Integration::verbose) 
      cerr << "TimerProfile_load big endian\n";
    fromBigEndian(&centrefreq,sizeof(centrefreq));
    fromBigEndian(&nbin,sizeof(nbin));
    fromBigEndian(&poln,sizeof(poln));
    fromBigEndian(&wt,sizeof(wt));
  }
  else {
    if (Integration::verbose)
      cerr << "TimerProfile_load little endian\n";
    fromLittleEndian(&centrefreq,sizeof(centrefreq));
    fromLittleEndian(&nbin,sizeof(nbin));
    fromLittleEndian(&poln,sizeof(poln));
    fromLittleEndian(&wt,sizeof(wt));
  }

  if (unsigned(nbin) != profile->get_nbin())
    throw Error (InvalidState, "TimerProfile_load",
		 "nbin=%d Profile::nbin=%d", nbin, profile->get_nbin());

  if (Integration::verbose)
    fprintf (stderr, "TimerProfile_load nbin:%d poln:%d wt:%f cfreq:%f\n",
	     nbin, poln, wt, centrefreq);

  // uncompress the data and read in as 2byte integers
  if (fcompread (nbin,profile->get_amps(),fptr,big_endian)!=0)
    throw Error (FailedCall, "TimerProfile_load", "fcompread data");

  profile -> set_weight (wt);
  profile -> set_centre_frequency (centrefreq);

}
