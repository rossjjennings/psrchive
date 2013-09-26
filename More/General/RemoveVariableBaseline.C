/***************************************************************************
 *
 *   Copyright (C) 2010 by Patrick Weltevrede
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/RemoveVariableBaseline.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"


double mean(float* x, int N){
   int i;
   float sum=0;
   for (i=0; i < N; i++){
	  sum+=x[i];
   }
   return sum/(float)N;
}


double covariance(float* x, float* y, int N){
   int i;
   float mx,my;
   float vx,vy;
   float *z;

   mx=mean(x,N);
   if (x==y){
	  my=mx;
   }else {
	  my=mean(y,N);
   }
   z = (float*)calloc(N,sizeof(float));
   for(i=0; i < N; i++){
	  z[i] = (x[i] - mx)*(y[i]-my);
   }
   return mean(z,N);


}

double variance(float* x, int N){
   return covariance(x,x,N);
}

void fitStraightLine(float* x, float* y, int N, float* m, float* c){
   float vx,cxy,mx,my;

   cxy = covariance(x,y,N);
   vx=variance(x,N);
   mx=mean(x,N);
   my=mean(y,N);
   *m = cxy/vx;
   *c = my - (*m) * mx;

   return;
}


void Pulsar::RemoveVariableBaseline::transform (Archive* archive)
{
   const unsigned nsub = archive->get_nsubint();
   const unsigned nchan = archive->get_nchan();
   const unsigned npol = archive->get_npol();

   float xvalues[10000];
   fprintf(stderr, "Entering Pulsar::RemoveVariableBaseline 4mar2010\n");

   for (unsigned isub=0; isub < nsub; isub++)
   {
	  Integration* subint = archive->get_Integration (isub);

	  for (unsigned ichan=0; ichan < nchan; ichan++)
	  {
		 for (unsigned ipol=0; ipol < npol; ipol++)
		 {
			Reference::To<Profile> profile = subint->get_Profile (ipol,ichan);

			const unsigned nbin = profile->get_nbin();
			float* data = profile->get_amps();

			float a, b;
			unsigned int j;
			for(j = 0; j < nbin; j++) {
			   xvalues[j] = j;
			}
			fitStraightLine(xvalues,data,nbin,&b,&a);
			for(j = 0; j < nbin; j++) {
			   data[j] -= a+xvalues[j]*b;
			}
		 }
	  }
   }
};



