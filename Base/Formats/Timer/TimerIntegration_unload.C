#include "TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "convert_endian.h"

//! unload the subint to file
void Pulsar::TimerIntegration::unload (FILE* fptr) const
{
  if (verbose == 3) cerr << "TimerIntegration::unload" << endl;

  struct mini* outmini = const_cast<struct mini*>(&mini);

  try {
    mini_toBigEndian(outmini);
    if (fwrite (outmini, 1, sizeof(mini), fptr) < 1)
      throw Error (FailedSys, "TimerIntegration::unload", "fwrite mini");
    mini_fromBigEndian(outmini);
  }
  catch (Error& error) {
    throw error += "TimerIntegration::unload mini";
  }

  try {
    if (verbose == 3) cerr << "TimerIntegration::unload writing weights" << endl;
    N_ToBigEndian (nchan, const_cast<float*>(&(wts[0])));
    fwrite(&(wts[0]),nchan*sizeof(float),1,fptr);
    N_FromBigEndian (nchan, const_cast<float*>(&(wts[0])));
  }
  catch (Error& error) {
    throw error += "TimerIntegration::unload wts";
  }
 
  try {
    if (verbose == 3) cerr << "TimerIntegration::unload writing med" << endl;
    for (unsigned ipol=0; ipol<npol; ipol++) {
      N_ToBigEndian (nchan, const_cast<float*>(&(med[ipol][0])));
      fwrite(&(med[ipol][0]),nchan*sizeof(float),1,fptr);
      N_FromBigEndian (nchan, const_cast<float*>(&(med[ipol][0])));
    }
  }
  catch (Error& error) {
    throw error += "TimerIntegration::unload med";
  }

 
  try {
    if (verbose == 3) cerr << "TimerIntegration::unload writing bpass" << endl; 
    for (unsigned ipol=0; ipol<npol; ipol++) {
      N_ToBigEndian (nchan, const_cast<float*>(&(bpass[ipol][0])));
      fwrite(&(bpass[ipol][0]),nchan*sizeof(float),1,fptr);
      N_FromBigEndian (nchan, const_cast<float*>(&(bpass[ipol][0])));
    }
  }
  catch (Error& error) {
    throw error += "TimerIntegration::unload bpass";
  }

  // And the profiles
  if(verbose == 3) cerr << "TimerIntegration::unload unloading profiles npol=" 
		   << npol << " nchan=" << nchan << endl;

  try {

    for (unsigned ipol=0; ipol<npol; ipol++)
      for (unsigned ichan=0; ichan<nchan; ichan++) {
	
	if (verbose == 3) cerr << "TimerIntegration::unload ipol=" << ipol 
			  << "\t ichan=" << ichan << "\r";
	
	TimerProfile_unload (fptr, profiles[ipol][ichan]);
	
      }
    
  }
  catch (Error& error) {
    throw error += "TimerIntegration::unload profiles";
  }
  
  if (verbose == 3) cerr << "TimerIntegration::unload exit" << endl;

}
