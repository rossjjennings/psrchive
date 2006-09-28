/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "convert_endian.h"

using namespace std;

//! unload the subint to file
void Pulsar::TimerIntegration::unload (FILE* fptr) const
{
  if (verbose) cerr << "TimerIntegration::unload" << endl;

  struct mini* outmini = const_cast<struct mini*>(&mini);

  mini_toBigEndian(outmini);
  if (fwrite (outmini, 1, sizeof(mini), fptr) < 1)
    throw Error (FailedSys, "TimerIntegration::unload", "fwrite mini");
  mini_fromBigEndian(outmini);

  if (verbose) cerr << "Pulsar::TimerIntegration::unload extra start offset="
                    << ftell(fptr) << endl;

  if (verbose) cerr << "TimerIntegration::unload writing " << nchan
                    << " weights" << endl;
  N_ToBigEndian (nchan, const_cast<float*>(&(wts[0])));
  fwrite(&(wts[0]),nchan*sizeof(float),1,fptr);
  N_FromBigEndian (nchan, const_cast<float*>(&(wts[0])));
 
  if (verbose) cerr << "TimerIntegration::unload writing " << npol
                    << "x" << nchan << " med" << endl;
  for (unsigned ipol=0; ipol<npol; ipol++) {
    N_ToBigEndian (nchan, const_cast<float*>(&(med[ipol][0])));
    fwrite(&(med[ipol][0]),nchan*sizeof(float),1,fptr);
    N_FromBigEndian (nchan, const_cast<float*>(&(med[ipol][0])));
  }

  if (verbose) cerr << "TimerIntegration::unload writing " << npol
                    << "x" << nchan << " bpass" << endl; 
  for (unsigned ipol=0; ipol<npol; ipol++) {
    N_ToBigEndian (nchan, const_cast<float*>(&(bpass[ipol][0])));
    fwrite(&(bpass[ipol][0]),nchan*sizeof(float),1,fptr);
    N_FromBigEndian (nchan, const_cast<float*>(&(bpass[ipol][0])));
  }

  if (verbose) cerr << "Pulsar::TimerIntegration::unload extra end offset="
                    << ftell(fptr) << endl;

  // And the profiles
  if(verbose) cerr << "TimerIntegration::unload unloading profiles npol=" 
		   << npol << " nchan=" << nchan << endl;

  for (unsigned ipol=0; ipol<npol; ipol++)
    for (unsigned ichan=0; ichan<nchan; ichan++) try {
	
      if (verbose) cerr << "TimerIntegration::unload ipol=" << ipol 
                        << "\t ichan=" << ichan << "\r";
	
      TimerProfile_unload (fptr, profiles[ipol][ichan]);
	
    }
    catch (Error& error) {
      error << "\n\tprofile[ipol=" << ipol << "][ichan=" << ichan << "]";
      throw error += "TimerIntegration::unload";
    }
  
  if (verbose) cerr << "TimerIntegration::unload exit" << endl;

}
