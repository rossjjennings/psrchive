/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/EPOSArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include "eposio.h"
#include "MJD.h"
#include <iomanip>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

void Pulsar::EPOSArchive::init ()
{
  // initialize the EPOSArchive attributes
  // These are any "extra" attributes that have been added to the
  // EPOSArchive class, but don't exist in BasicArchive.  Typically
  // this would be some info specific to this file format.
  current_record = 0;
  ref_syncusec = 0;
}

Pulsar::EPOSArchive::EPOSArchive()
{
  if (verbose > 2)
	cerr << "Entering into --> " << __func__ << endl;
  
  init ();
}

Pulsar::EPOSArchive::~EPOSArchive()
{
  // destroy any EPOSArchive-specific resources
}

Pulsar::EPOSArchive::EPOSArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::EPOSArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::EPOSArchive::EPOSArchive (const EPOSArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::EPOSArchive construct copy EPOSArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::EPOSArchive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::EPOSArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::EPOSArchive::copy dynamic cast call" << endl;
  
  const EPOSArchive* like_me = dynamic_cast<const EPOSArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::EPOSArchive::copy another EPOSArchive" << endl;

  eposhdr = like_me->eposhdr;
  eposdata = like_me->eposdata;

  // copy EPOSArchive attributes
}

Pulsar::EPOSArchive* Pulsar::EPOSArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::EPOSArchive::clone" << endl;
  return new EPOSArchive (*this);
}

MJD Pulsar::EPOSArchive::get_startdate (const char *filename, eposhdr_t *eh, unsigned subint)
{
  double CEToffset = 0.0;
  char mjdstring[100];
  //double actper = (double)(eh->iactper1*(7 + eh->iactper2)/(1.0*eh->masterref));
  double mjd_offset = eh->isyncday + 40587.5;

  int cur_syncsec = eh->isyncsec;
  int cur_syncmin = eh->isyncmin;
  int cur_synchr = eh->isynchr;

  // for dates before 00:00:00 hrs 07-05-2003, times are in CET.
  // So substract 1 hour from the time read off the header to get UTC. Refer to Axel's EPOS matlab routine
  if (mjd_offset < 52766.0) CEToffset = 3600.0;
  
  // Note: counts up to 2**31, and then overflows
  int syncusec = ( (eh->isyncusec & 0x000000ff) << 16) | ( (eh->isyncusec & 0x00007f00) << 16) | \
    ( (eh->isyncusec & 0x00ff0000) >> 16) | ( (eh->isyncusec & 0xff000000) >> 16);

  // deal with overflow, counter is 31-bits
  // First, get syncusec from the previous subint
  unsigned prev_subint = 0;
  int prev_syncusec = 0;
  int delta = 0;

  if ( subint > 0) {
	prev_subint = subint - 1;
	if ( !get_hdr(filename, eh, prev_subint, verbose))
	  throw Error (InvalidState, "Pulsar::EPOSArchive::get_startdate",
				   "cannot get header from File %s for subint %d\n", filename, prev_subint);
	prev_syncusec = ( (eh->isyncusec & 0x000000ff) << 16) | ( (eh->isyncusec & 0x00007f00) << 16) | \
	  ( (eh->isyncusec & 0x00ff0000) >> 16) | ( (eh->isyncusec & 0xff000000) >> 16);
  }

  delta = syncusec - prev_syncusec;

  // We have an overflow. Compute the increment anew.
  if ( delta < 1 )  delta = (0x7fffffff - prev_syncusec) + syncusec;
  else delta = syncusec - prev_syncusec;

  if (subint == 0 ) ref_syncusec = syncusec;
  else ref_syncusec += delta;

  double sync_time_s = (ref_syncusec/1000000.0) + cur_syncsec + 60*cur_syncmin + 3600*cur_synchr - CEToffset;
  double fullmjd = mjd_offset + (sync_time_s/86400.0);
  
  // convert start time to C-string, so that MJD() can convert it properly
  memset(&mjdstring[0],'\0',100);

  sprintf(&mjdstring[0], "%39.34f", fullmjd );
  return MJD (mjdstring);
  
}

double Pulsar::EPOSArchive::get_intlen (eposhdr_t *eposhdr)
{
  double duration;

  double period = get_foldperiod(eposhdr);

  duration = (double)eposhdr->nint * period;

  return duration;
}

string Pulsar::EPOSArchive::get_source (eposhdr_t *eposhdr)
{
  string mystr = reinterpret_cast<char *> (eposhdr->source);

  string rstr = mystr.substr(4);
  
  return rstr;
}

double Pulsar::EPOSArchive::get_foldperiod (eposhdr_t *eposhdr)
{
  // Check Axel's Matlab routines. 7 cycles of latency are added to actper2
  double actper = (double)eposhdr->iactper1*(double)(7 + eposhdr->iactper2)/(double)eposhdr->masterref;
  return actper;
}

Pulsar::Integration*
Pulsar::EPOSArchive::new_Integration (const Integration* subint)
{
  if (verbose == 3)
    cerr << "Pulsar::EPOSArchive::new_Integration" << endl;

  BasicIntegration* integration;

  if (subint)
    integration = new BasicIntegration (subint);
  else
    integration = new BasicIntegration;

  if (!integration)
    throw Error (BadAllocation, "Pulsar::EPOSArchive::new_Integration");

  return integration;
}


void Pulsar::EPOSArchive::load_header (const char* filename)
{

  // determine if the file is from EPOS.  If not, throw a error:

  if (check_file(filename, &eposhdr, verbose) < 0) 
    throw Error (InvalidState, "Pulsar::EPOSArchive::load_header",
        "File (%s) is not correct type", filename);

  // Next, set the class attributes using data from the file.
  // Number of polarizations, channels, and subintegrations in file.
  set_npol(1);
  set_nchan(1);

  int nsub = get_nsubints(filename);

  set_nsubint(nsub);

  // Number of bins per pulse period in folded profiles. Always 1024 for EPOS.
  set_nbin(1024);

  // Polarization state, see Util/genutil/Types.h for allowed values
  set_state(Signal::Intensity);

  // Data scale, values also defined in Types.h
  set_scale(Signal::FluxDensity);

  // Source name
  string src = get_source(&eposhdr);
  set_source(src);

  // TEMPO telescope site code
  set_telescope("g");

  // Observation type (PSR or CAL), again see Types.h
  set_type(Signal::Pulsar);

  // Total bandwidth, MHz. Negative value denotes reversed band.
  set_bandwidth(140.0);

  // Center frequency, MHz.  Note correct spelling of "center" ;)
  set_centre_frequency(1410.0);

  // Dispersion measure, pc/cm^3.
  set_dispersion_measure(71.025);

  set_dedispersed(false); 
  set_faraday_corrected(false);
  set_poln_calibrated(false);
}

Pulsar::Integration*
Pulsar::EPOSArchive::load_Integration (const char* filename, unsigned subint)
{

  // Data structure containing subint info/data that we will return.
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // Allocates space for data using the values set earlier by load_header.
  resize_Integration(integration);

  // read the header block for this subint

  if ( !get_hdr(filename, &eposhdr, subint, verbose))
	throw Error (InvalidState, "Pulsar::EPOSArchive::load_Integration",
				 "cannot get header from File %s for subint %d\n", filename, subint);

  MJD myepoch = get_startdate(filename, &eposhdr, subint);
  integration->set_epoch(myepoch);

  if (verbose > 1)
	cerr << "epoch=" << setprecision(21) << myepoch << endl;

  double myduration = get_intlen(&eposhdr);
  integration->set_duration(myduration); // Integration time, s.

  double myfoldperiod = get_foldperiod(&eposhdr);
  integration->set_folding_period(myfoldperiod); // Pulsar period, s.

  // If the "no_amps" flag is set, the actual data is not called for, 
  // so we can exit early.  (Trying to actually load the data 
  // in this case will cause errors or segfaults.)
  if (Profile::no_amps) return integration;

  // Load the actual data for each pol, channel from the file.
  float *data = new float[nbin]; // Temporary storage space

  unsigned i;

  if ( !get_data(filename, &eposdata, subint, verbose)) 
	throw Error (InvalidState, "Pulsar::EPOSArchive::load_Integration",
                 "cannot get data from File %s for subint %d\n", filename, subint);

  for ( i=0;i<nbin;i++) data[i] = (float)eposdata.data0[i] + (float)eposdata.data1[i];

  // find average and remove baseline

  //  float total=0, avg=0;
  //for (i=0;i<300;i++) total = total + data[i];
  //avg = total/300.0;

  //for ( i=0;i<nbin;i++) data[i] = data[i] - avg;
  //data[nbin-1] =  data[nbin-3] ;
  //data[nbin-2] =  data[nbin-4] ;

  for (unsigned ichan=0; ichan<nchan; ichan++)
    integration->set_centre_frequency(ichan, 1410.0);

  for (unsigned ipol=0; ipol<npol; ipol++) {
    for (unsigned ichan=0; ichan<nchan; ichan++) {
      // Load data for ipol, ichan into data array here.
      // Put data in integration structure:
      integration->get_Profile(ipol,ichan)->set_amps(data);
    }
  }

  // Unallocate temp space
  delete [] data;

  return integration;
}

void Pulsar::EPOSArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and EPOSArchive attributes as well as
  // BasicIntegration attributes and data to filename.

  // This does not need to be filled in if you never need to save files
  // in this data format.  Assuming this is the case, it's best to throw 
  // an error here:
  throw Error (InvalidState, "Pulsar::EPOSArchive::unload_file",
      "unload not implemented for EPOSArchive");

}

string Pulsar::EPOSArchive::Agent::get_description () 
{
  return "EPOS Archive Version 1.0";
}

bool Pulsar::EPOSArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by EPOS Archive, this method should return true.
  
  // One way to do this is to try to call the load_header method and
  // check for errors:
  try {
    EPOSArchive archive;
    archive.load_header(filename);
    return true;
  }
  catch (Error &e) {
    return false;
  }

  return false;
}

