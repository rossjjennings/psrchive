#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "fcomp.h"
#include "convert_endian.h"

#include <stdio.h>
#ifdef sun
#include <ieeefp.h>
#endif

int Pulsar::TimerProfile_poln (Signal::Component state)
{
  switch (state) {

  case Signal::Si:
    return 0;

  case Signal::XX:
  case Signal::LL:
    return 1;

  case Signal::YY:
  case Signal::RR:
    return 2;

  case Signal::ReXY:
  case Signal::ReLR:
    return 3;

  case Signal::ImXY:
  case Signal::ImLR:
    return 4;

  case Signal::Sq:
    return 5;

  case Signal::Su:
    return 6;

  case Signal::Sv:
    return 7;

  case Signal::Inv:
    return 8;

  case Signal::DetRho:
    return 9;

  default:
    return -1;
  }
}

/*!
  \pre The Profile must have been resized before calling this function
*/
void Pulsar::TimerProfile_unload (FILE* fptr, const Profile* profile)
{
  float centrefreq = profile -> get_centre_frequency();
  int   nbin       = profile -> get_nbin();
  int   poln       = TimerProfile_poln (profile -> get_state());
  float wt         = profile -> get_weight ();

  if (nbin < 1)
    throw Error (InvalidParam, "Pulsar::TimerProfile_unload",
                 "invalid Profile::nbin=%d", nbin);

  const float* amps = profile->get_amps();
  for (int ibin=0; ibin < nbin; ibin++)
    if (!finite(amps[ibin]))
      throw Error (InvalidParam, "Pulsar::TimerProfile_unload",
                   "amps[%d]=%f is not finite", ibin, amps[ibin]);

  toBigEndian(&centrefreq, sizeof(centrefreq));
  toBigEndian(&nbin, sizeof(nbin));
  toBigEndian(&poln, sizeof(poln));
  toBigEndian(&wt,   sizeof(wt));

  if (TimerIntegration::verbose)
    cerr << "Pulsar::TimerProfile_unload start offset=" << ftell(fptr) << "\r";

  //Write out the values
  if (fwrite (&centrefreq, sizeof(centrefreq),1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_unload", "fwrite centrefreq");
  if (fwrite (&nbin, sizeof(nbin), 1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_unload", "fwrite nbin");
  if (fwrite (&poln, sizeof(poln), 1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_unload", "fwrite poln");
  if (fwrite (&wt,   sizeof(wt),   1,fptr) < 1)
    throw Error (FailedSys, "TimerProfile_unload", "fwrite wt");

  fromBigEndian(&nbin, sizeof(nbin));

  if (TimerIntegration::verbose)
    cerr << "Pulsar::TimerProfile_unload fcompwrite data nbin=" << nbin << "\r";

  // Compress the data and write out as 2byte integers
  if (fcompwrite (nbin,profile->get_amps(),fptr) != 0)
    throw Error (FailedCall, "TimerProfile_unload", "fcompwrite data");

  if (TimerIntegration::verbose)
    cerr << "Pulsar::TimerProfile_unload end offset=" << ftell(fptr) << "\r";

}

