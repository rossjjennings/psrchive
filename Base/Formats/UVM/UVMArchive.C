/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/UVMArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include "uvmio.h"
#include "utc.h"

#include <fstream>
#include <string.h>

using namespace std;

Pulsar::Option<bool> Pulsar::UVMArchive::include_offpulse_region
(
 "UVMArchive::include_offpulse_region", true,

 "Load phase bins identified as part of off-pulse baseline [boolean]",

 "Self explanatory."
);

void Pulsar::UVMArchive::init ()
{
  integration_length = 0.0;
  period = 1.0;
}

Pulsar::UVMArchive::UVMArchive()
{
  init ();
}

Pulsar::UVMArchive::~UVMArchive()
{
  // destroy any UVMArchive resources
}

Pulsar::UVMArchive::UVMArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::UVMArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::UVMArchive::UVMArchive (const UVMArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::UVMArchive construct copy UVMArchive" << endl;

  init ();
  Archive::copy (arch);
}

void Pulsar::UVMArchive::copy (const Archive& archive) 
{
  if (verbose > 2)
    cerr << "Pulsar::UVMArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive);

  if (verbose > 2)
    cerr << "Pulsar::UVMArchive::copy dynamic cast call" << endl;

  const UVMArchive* like_me = dynamic_cast<const UVMArchive*>(&archive);
  if (!like_me)
    return;

  if (verbose > 2)
    cerr << "Pulsar::UVMArchive::copy another UVMArchive" << endl;

  integration_length = like_me -> integration_length;
}

Pulsar::UVMArchive* Pulsar::UVMArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::UVMArchive::clone" << endl;
  return new UVMArchive (*this);
}

void Pulsar::UVMArchive::load_header (const char* filename)
{
  int newscan = 0;

  uvm_header* header = new uvm_header;
  header_ptr = header;
  
  if (uvm_getheader (filename, &program, &newscan, header) < 0)
    throw Error (InvalidState, "Pulsar::UVMArchive::load_header",
		 "%s is not a UVM archive", filename);

  cerr << "newscan=" << newscan << endl;

  char buffer[80];
  uvm_get_source (header, buffer);
  source = buffer;

  uvm_get_observatory (header, buffer);
  telescope = buffer;
  telescope = "Arecibo";

  uvm_get_windows (header, buffer);
  cerr << "Pulsar::UVMArchive::load_header windows=" << buffer
       << " nwins=" << header->nwins << endl;

  string windows = buffer;

  centre_frequency = header->obfreq;
  bandwidth = header->abandwd;

  npol = header->anumsbc;
  if (npol == 4)
    set_state (Signal::Stokes);

  cerr << "anumsbc=" << header->anumsbc << endl;
  cerr << "aendscan=" << header->aendscan << endl;
  cerr << "samint=" << header->samint << endl;
  cerr << "apbins=" << header->apbins << endl;
  cerr << "nseq=" << header->nseq << endl;
  cerr << "baseval=" << header->baseval << endl;
  
  /*
  if (header->apoladd)
    npol = 1;
  */

  dispersion_measure = header->adm;
  rotation_measure = header->arm;

  if (header->nwins == 0)
  {
    nbin = header->apbins;
    cerr << "nbin = apbins = " << nbin;
  }
  else
  {
    nbin = 0;
    for (int iwin=0; iwin < header->nwins; iwin++)
      if (include_offpulse_region || windows[iwin] != 'N')
      {
        cerr << "nbin += " <<  header->nwbins[iwin] << endl;
        nbin += header->nwbins[iwin];
      }
  }

  nchan = 1;
  
  if (Profile::no_amps)
  {
    cerr << "no amps" << endl;
    return;
  }

  cerr << "date (dddyy)=" << header->date << endl;
  cerr << "start sec=" << header->startime << endl;

  utc_t utc;
  utc.tm_yday = header->date / 100;
  utc.tm_year = 1900 + header->date % 100;
  utc.tm_min = header->scantime / 60;
  utc.tm_sec = header->scantime - utc.tm_min * 60;
  utc.tm_hour = utc.tm_min / 60;
  utc.tm_min -= utc.tm_hour * 60;

  MJD mjd_start (utc);

  // load all BasicIntegration attributes and data from filename
  uvm_data data;
  unsigned loaded_subints = 0;

  while (uvm_getdata (program, header, &data) >= 0)
  {
    loaded_subints ++;
    resize (loaded_subints);

    Integration* integration = get_Integration(loaded_subints-1);

    // cerr << "start usec=" << header->scantime << endl;
    epoch = mjd_start + header->scantime * 1e-6;

    /* from readscn.f:
    c		samint	 f9.7	interval between samples in seconds (average)
    c				sampled fraction of period in SP program */

    double interval = header->period * header->samint;
    
    integration->set_folding_period (header->period);
    integration->set_gate_duty_cycle (header->samint);
    integration->set_duration (interval);
    integration->set_epoch (epoch);
    integration->set_centre_frequency(0, centre_frequency);
    
    // scaleI is the scale to convert to microJy
    double scale = header->scaleI * 1e-3;
    double offset = 0;

    for (unsigned ipol=0; ipol < get_npol(); ipol++)
    {
      float* amps = integration->get_Profile(ipol, 0) -> get_amps();

      if (ipol == 0)
	offset = header->baseval;
      else
	offset = 0;

      if (header->nwins == 0)
      {      
        for (unsigned ibin=0; ibin < get_nbin(); ibin++)
        {
	  amps[ibin] = (data.data[ipol][ibin] + offset) * scale;
        }
      }
      else
      {
        unsigned data_ibin = 0;
        unsigned amps_ibin = 0;
        for (int iwin=0; iwin < header->nwins; iwin++)
          if (include_offpulse_region || windows[iwin] != 'N')
          {
            for (unsigned ibin = 0; ibin < header->nwbins[iwin]; ibin++)
            {
              amps[amps_ibin] = (data.data[ipol][data_ibin] + offset) * scale;
              data_ibin ++;
              amps_ibin ++;
            }
          }
          else
            data_ibin += header->nwbins[iwin];
      }
    }
  }
}

Pulsar::Integration*
Pulsar::UVMArchive::load_Integration (const char* filename, unsigned subint)
{
  throw Error (InvalidState, "Pulsar::UVMArchive::load_Integration",
	       "not implemented");
}



string Pulsar::UVMArchive::Agent::get_description () 
{
  return "UVM Archive";
}


bool Pulsar::UVMArchive::Agent::advocate (const char* filename) try
{
  int program = 0;
  int newscan = 0;
  uvm_header header;
  
  if (uvm_getheader (filename, &program, &newscan, &header) < 0)
    return false;

  return true;
}
catch (Error& e) 
{
  if (Archive::verbose > 2)
    cerr << "UVM load failed due to: " << e << endl;

  return false;
}
catch (std::exception& e)
{
  return false;
}
