#include <assert.h>

#include "TimerIntegration.h"
#include "Pulsar/Profile.h"
#include "Error.h"

#include "convert_endian.h"

/* assumes that all profiles have equal nbins */
void unpackprofiles (vector<vector<Reference::To<Pulsar::Profile> > >& profs,
		     int npol, int nsub, int nbin,
		     unsigned short int packed[],
		     float scale, float offset)
{
  int ipol, isub, ibin, index;

  index = 0;
  for (ipol=0; ipol<npol; ipol++)
    for (isub=0; isub<nsub; isub++) {
      float* amps = profs[ipol][isub]->get_amps();
      for (ibin=0; ibin<nbin; ibin++) {
	amps[ibin] = offset + packed[index] * scale;
	index ++;
      }
    }
}

//! load the subint from file
void Pulsar::TimerIntegration::load (FILE* fptr, int extra, bool big_endian)
{
  unsigned i,j;
  float scale, offset;

  // read in the mini header
  if (fread (&mini,sizeof(mini),1,fptr) < 1)
    throw Error (FailedSys, "TimerIntegration::load", "fread mini");

  if (big_endian)
    mini_fromBigEndian(&mini);
  else
    mini_fromLittleEndian(&mini);

  // If new-style, load everything - otherwise set to default values.
  // new_style was when we started storing weights for each channel
  // and the bandpass and median (called peak here) arrays.
  // NOTE: This test is also needed for loading filter bank data 
  // since none of these three arrays is computed for filter bank archives.
  // (meaning that extra variable should be 0 for FB archives).

  if (extra) {

    if (verbose)
      cerr << "TimerIntegration::load in the new style" << endl;

    if ( fread (&(wts[0]),nchan*sizeof(float),1,fptr) != 1 )
      throw Error (FailedSys, "TimerIntegration::load", "fread wts");

    if (big_endian)
      N_FromBigEndian (nchan, &(wts[0]));
    else
      N_FromLittleEndian (nchan, &(wts[0]));

    for (i=0;i<npol;i++) {
      if (fread(&(med[i][0]),nchan*sizeof(float),1,fptr)!=1)
	throw Error (FailedSys,"TimerIntegration::load", "fread medians");
      if (big_endian)
	N_FromBigEndian (nchan, &(med[i][0]));
      else
	N_FromLittleEndian (nchan, &(med[i][0]));
    }
    for (i=0;i<npol;i++) {
      if(fread(&(bpass[i][0]),nchan*sizeof(float),1,fptr)!=1)
	throw Error (FailedSys,"TimerIntegration::load", "fread bpass");
      if (big_endian)
	N_FromBigEndian (nchan, &(bpass[i][0]));
      else
	N_FromLittleEndian (nchan, &(bpass[i][0]));
    }
    /* new style */
    if (verbose) cerr << "TimerIntegration::load loading profiles\n";
    // Read the array of profiles
    for(i=0; i<npol;i++)
      for(j=0; j<nchan;j++) 
	TimerProfile_load (fptr, profiles[i][j], big_endian);

  } 
  else {
    if (verbose)
      cerr << "TimerIntegration::load in the old style." << endl;

    // No weights available.
    for(i=0;i<npol;i++)
      for (j=0; j<nchan; j++) {
	wts[j]      = 1.0;
	med[i][j]   = 1.0;
	bpass[i][j] = 1.0;
      }

    Signal::Basis basis = get_basis();
    Signal::State state = get_state();

    double centrefreq = get_centre_frequency();
    double bw = get_bandwidth();

    for(i=0; i<npol; i++)
      for(j=0; j<nchan; j++) {
	// already set during resize in TimerArchive::subint_load
        // profiles[i][j]->nbin = nbin;
        profiles[i][j]->set_weight (1.0);
        profiles[i][j]->set_state (Signal::get_Component (basis, state, i));
	double cfreq = centrefreq-(bw/2.0)+(j+0.5)*bw/nchan;
        profiles[i][j]->set_centre_frequency (cfreq);
      }

    int npts = nbin*nchan*npol;
    unsigned short int *packed = new unsigned short int [npts];
    assert (packed != NULL);

    // old style + Filter bank
    if(fread(&scale, sizeof(scale), 1,fptr)!=1)
      throw Error (FailedSys,"TimerIntegration::load", "fread scale");

    if(fread(&offset,sizeof(offset),1,fptr)!=1)
      throw Error (FailedSys,"TimerIntegration::load", "fread offset");

    size_t ptsrd = fread(packed, sizeof(unsigned short int), npts, fptr);

    if (int(ptsrd) < npts) {

      ErrorCode code = InvalidState;
      if (ferror(fptr))
	code = FailedSys;

      throw Error (code, "TimerIntegration::load",
		   " read %d/%d pts", ptsrd, npts);

    }

    if (big_endian) {
      FromBigEndian (scale);
      FromBigEndian (offset);
      N_FromBigEndian (npts, packed);
    }
    else {
      FromLittleEndian (scale);
      FromLittleEndian (offset);
      N_FromLittleEndian (npts, packed);
    }

    if (verbose) 
      fprintf (stderr, "TimerIntegration::load scale:%f offset:%f\n",
			  scale, offset);

    if (scale == 0.0)
      throw Error (InvalidState, "TimerIntegration::load",
		   "Corrupted scale factor");

    unpackprofiles (profiles, npol, nchan, nbin, packed, scale, offset);

    delete [] packed;
  }    // End old style loading

}      
