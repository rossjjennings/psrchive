#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "convert_endian.h"
#include "fcomp.h"

#include <stdio.h>

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
		 "nbin=%d != Profile::nbin=%d", nbin, profile->get_nbin());

  if (Integration::verbose)
    cerr << "TimerProfile_load nbin=" << nbin << " poln=" << poln
         << "wt=" << wt << " cfreq=" << centrefreq << endl;

  // uncompress the data and read in as 2byte integers
  if (fcompread (nbin,profile->get_amps(),fptr,big_endian)!=0)
    throw Error (FailedCall, "TimerProfile_load", "fcompread data");

  profile -> set_weight (wt);
  profile -> set_centre_frequency (centrefreq);

}
