/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/WAPPArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"
#include "FTransform.h"

#include "wapp_header.h"

using namespace std;

void Pulsar::WAPPArchive::init ()
{
  // initialize the WAPPArchive attributes
  rawhdr=NULL;
  hdr=NULL;
  wapp_hdr_version=0;
  wapp_hdr_size=0;
  wapp_ascii_hdr_size=0;
  wapp_file_size=0;
  raw_data_is_lags=0;
}

Pulsar::WAPPArchive::WAPPArchive()
{
  init ();
}

Pulsar::WAPPArchive::~WAPPArchive()
{
  // destroy any WAPPArchive-specific resources
  if (rawhdr!=NULL) { delete [] rawhdr; }
}

Pulsar::WAPPArchive::WAPPArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::WAPPArchive::WAPPArchive (const WAPPArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive construct copy WAPPArchive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::WAPPArchive::WAPPArchive (const Archive& arch, 
                                        const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive construct extract Archive" << endl;

  init ();
  Archive::copy (arch, subints);
}


void Pulsar::WAPPArchive::copy (const Archive& archive, 
                                   const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive, subints);

  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive::copy dynamic cast call" << endl;
  
  const WAPPArchive* like_me = dynamic_cast<const WAPPArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive::copy another WAPPArchive" << endl;

  // copy WAPPArchive attributes 
}

Pulsar::WAPPArchive* Pulsar::WAPPArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive::clone" << endl;
  return new WAPPArchive (*this);
}

Pulsar::WAPPArchive* 
Pulsar::WAPPArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive::extract" << endl;
  return new WAPPArchive (*this, subints);
}

void Pulsar::WAPPArchive::load_header (const char* filename)
{
  // load all BasicArchive and WAPPArchive attributes from filename
  if (verbose > 2)
    cerr << "Pulsar::WAPPArchive::load_header" << endl;
  
  // Open file
  FILE *f = fopen(filename, "r");
  if (!f)  
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_header",
        "fopen(%s)", filename);

  // First we skip the ascii part of the WAPP header.  We could
  // save this into a buffer, I guess..
  char ctmp;
  wapp_ascii_hdr_size=0;
  while ((ctmp=fgetc(f))!='\0') {
    wapp_ascii_hdr_size++;
    if (ctmp==EOF) {
      fclose(f);
      throw Error (InvalidState, "Pulsar::WAPPArchive::load_header",
          "Couldn't find end of ASCII header");
    }
  }
  wapp_ascii_hdr_size++; // Include null byte in this count

  // Next two ints are header version and size
  // TODO : fix byte-swap if needed
  int rv;
  rv = fread(&wapp_hdr_version, sizeof(uint32_t), 1, f);
  if (rv!=1) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_header",
        "fread(hdr_version)");
  }
  rv = fread(&wapp_hdr_size, sizeof(uint32_t), 1, f);
  if (rv!=1) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_header",
        "fread(hdr_size)");
  }

  // Check for known version
  int version_ok=0;
  if (wapp_hdr_version==9) { version_ok=1; }
  else { version_ok=0; }
  if (!version_ok) {
    fclose(f);
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_header",
        "Unsupported header version (%d)", wapp_hdr_version);
  }

  // Check that header size seems reasonable
  if (wapp_hdr_size>8192) {
    fclose(f);
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_header",
        "Incorrect header size? (%d B)", wapp_hdr_size);
  }

  // Alloc mem, read in full header
  rawhdr = new char[wapp_hdr_size];
  rv = fseek(f, -2*sizeof(uint32_t), SEEK_CUR);
  if (rv<0) { 
    fclose(f);
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_header",
        "fseek()");
  }
  rv = fread(rawhdr, wapp_hdr_size, 1, f);
  if (rv!=1) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_header",
        "fread(rawhdr)");
  }
  hdr = (struct WAPP_HEADER *)rawhdr;

  // Find out how big the file is
  rv = fseek(f, 0, SEEK_END);
  if (rv<0) { 
    fclose(f);
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_header",
        "fseek(EOF)");
  }
  wapp_file_size = ftell(f);
  fclose(f);
  if (wapp_file_size<0) 
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_header",
        "ftell()");

  // Check that this is a folding-mode file.
  if (strncmp(hdr->obs_type, "PULSAR_FOLDING", 15)!=0) 
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_header",
        "Only fold-mode WAPP file are supported (read obs_type=%s)",
        hdr->obs_type);

  // TODO : byteswap!  WAPP files are little-endian.

  // Convert header info to psrchive format.  Check values 
  // where appropriate.

  // Number of polarizations, channels, and subintegrations in file.
  if ((hdr->nifs==1)||(hdr->nifs==2)||(hdr->nifs==4)) {
    set_npol(hdr->nifs); 
  } else {
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_header",
        "Invalid nifs=%d", hdr->nifs);
  }
  set_nchan(hdr->num_lags);

  // How many dumps, ACFs versus spectra
  size_t dump_size_bytes=0;
  if (hdr->lagformat==WAPP_FLOATLAGS) { 
    dump_size_bytes = 4 * hdr->num_lags * hdr->nbins * hdr->nifs;
    raw_data_is_lags = 1;
  } else if (hdr->lagformat==WAPP_FLOATSPEC) {
    dump_size_bytes = 4 * hdr->num_lags * hdr->nbins * hdr->nifs;
    raw_data_is_lags = 0;
  } else {
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_header",
        "Unexpected lagformat=%d", hdr->lagformat);
  }
  set_nsubint((wapp_file_size-wapp_hdr_size-wapp_ascii_hdr_size)
      / dump_size_bytes);

  // Number of bins per pulse period in folded profiles.
  set_nbin(hdr->nbins);

  // Polarization state
  if (hdr->nifs==1) {
    if (hdr->sum) {
      set_state(Signal::Intensity);
    } else {
      set_state(Signal::PP_State); // but no one would do this, right?
    }
  } else if (hdr->nifs==2) {
    set_state(Signal::PPQQ);
  } else if (hdr->nifs==4) {
    set_state(Signal::Coherence);
  } else {
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_header",
        "Invalid nifs=%d", hdr->nifs);
  }

  // Data scale
  set_scale(Signal::FluxDensity);

  // Source name
  set_source(hdr->src_name);

  // TEMPO telescope site code
  set_telescope("3"); // WAPPs only exist at Arecibo...

  // Observation type (PSR or CAL)
  // Look for 25 Hz constant folding period to determine if 
  // this is a cal scan.
  if ((fabs(hdr->psr_f0[0]-25.0) < 1e-5) && (hdr->num_coeffs[0]==0)) {
    set_type(Signal::PolnCal); 
  } else {
    set_type(Signal::Pulsar);
  }

  // Total bandwidth, MHz. Negative value denotes reversed band.
  // Apparently the freqinversion and iflo_flip flags are cumuulative,
  // so we multiply at each step if needed.
  int bw_sign = 1;
  if (verbose>2)
    cerr << "WAPP IF/LO Flips: " << hdr->freqinversion 
      << " " << hdr->iflo_flip[0] 
      << " " << hdr->iflo_flip[1] << endl;
  if (hdr->freqinversion==1) { bw_sign*=-1; }
  if (hdr->iflo_flip[0]&&hdr->iflo_flip[1]) { bw_sign*=-1; }
  if (hdr->iflo_flip[0]!=hdr->iflo_flip[1]) 
    cerr << "Warning: WAPP polns have inconsistent iflo_flip." << endl;
  set_bandwidth((double)bw_sign * hdr->bandwidth);

  // Center frequency, MHz.
  set_centre_frequency(hdr->cent_freq);

  // Dispersion measure, pc/cm^3.
  set_dispersion_measure(hdr->psr_dm);

  // Flags telling whether the data have had any of these operations
  // done to it already.  "dedispersed" refers to the inter-channel
  // dispersion delay, not in-channel coherent dedisp.
  set_dedispersed(false); 
  set_faraday_corrected(false);
  set_poln_calibrated(false);

  // Load in polycos
  if (get_type()==Signal::Pulsar) { load_polycos(); }

}

void Pulsar::WAPPArchive::load_polycos() 
{
  // Find how many sets (max 16) are loaded in the file
  int num_polyco_sets = 0;
  int pmap[16];
  for (int i=0; i<16; i++) {
    if (hdr->num_coeffs[i]!=0) { 
      pmap[num_polyco_sets] = i;
      num_polyco_sets++; 
    }
  }

  // If no polycos return
  if (num_polyco_sets==0) {
    if (verbose>2) 
      cerr << "Warning: No polycos found in WAPP file." << endl;
    return;
  }

  // Resize hdr_polyco struct
  hdr_polyco.pollys.resize(num_polyco_sets);

  // nspan is not recorded, so we'll try to figure it out here
  double span = 60.0;  // Guess 1 hour
  if (num_polyco_sets>1) {
    // Set to diff between ref epochs
    span = hdr->poly_tmid[pmap[1]] - hdr->poly_tmid[pmap[0]];
    span *= 24.0*60.0;
  } 

  // Load info into polyco struct using Expert interface
  for (int ipoly=0; ipoly<num_polyco_sets; ipoly++) {
    polynomial::Expert poly(&hdr_polyco.pollys[ipoly]);
    int ncoef = hdr->num_coeffs[pmap[ipoly]];
    poly.set_tempov11(true); 
    poly.set_telescope(get_telescope().at(0));
    poly.set_freq(get_centre_frequency());
    poly.set_reftime(hdr->poly_tmid[pmap[ipoly]]);
    poly.set_refphase(hdr->rphase[pmap[ipoly]]);
    poly.set_reffrequency(hdr->psr_f0[pmap[ipoly]]);
    poly.set_nspan(span);
    poly.set_dm(hdr->psr_dm);
    poly.set_ncoef(ncoef);
    poly.set_doppler_shift(0.0);  // it's not in the file..
    poly.set_psrname(get_source());
    poly.set_binary(false); // binary info shouldn't matter here..
    poly.set_binph(0.0);  
    double *coefs = poly.get_coefs();
    for (int icoef=0; icoef<ncoef; icoef++) {
      coefs[icoef] = hdr->coeff[pmap[ipoly]*12 + icoef];
    }
  }
  
}

Pulsar::Integration*
Pulsar::WAPPArchive::load_Integration (const char* filename, unsigned subint)
{
  // load all BasicIntegration attributes and data from filename.

  // Data structure containing subint info/data that we will return.
  Pulsar::BasicIntegration* integration = new BasicIntegration;

  // Allocates space for data using the values set earlier by 
  // load_header.
  resize_Integration(integration);

  // Convert date/time strings given in header into MJD
  // object.
  struct tm obs_date_greg;
  int rv = sscanf(hdr->obs_date, "%4d%2d%2d", 
      &obs_date_greg.tm_year, &obs_date_greg.tm_mon,
      &obs_date_greg.tm_mday);
  obs_date_greg.tm_year -= 1900;
  obs_date_greg.tm_mon -= 1;
  if (rv!=3) 
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_Integration",
        "Error converting obs_date string (rv=%d, obs_date=%s)", 
        rv, hdr->obs_date);
  rv = sscanf(hdr->start_time, "%2d:%2d:%2d", 
      &obs_date_greg.tm_hour, &obs_date_greg.tm_min, 
      &obs_date_greg.tm_sec);
  if (rv!=3) 
    throw Error (InvalidState, "Pulsar::WAPPArchive::load_Integration",
        "Error converting start_time string (rv=%d, start_time=%s)", 
        rv, hdr->start_time);
  MJD epoch(obs_date_greg); 

  // epoch now refers to the time of the first sample.  We need to
  // use the included polycos to get reference epochs for each
  // subint.  
  Phase midphase=0.0;
  double midfreq=0.0;
  epoch += ((double)subint + 0.5) * hdr->dumptime; // Middle of subint
  if (get_type()==Signal::Pulsar) {
    if (hdr_polyco.pollys.size()==0)
      throw Error (InvalidState, "Pulsar::WAPPArchive::load_Integration",
          "No polyco sets found -- can't compute timestamps!");
    midphase = hdr_polyco.phase(epoch);
    midfreq = hdr_polyco.frequency(epoch);
    epoch -= midphase.fracturns() / midfreq;
  } else {
    // Constant folding period (cals)
    midfreq = hdr->psr_f0[0];
  }
  integration->set_epoch(epoch);
  integration->set_duration(hdr->dumptime);
  integration->set_folding_period(1.0/midfreq);

  // Set RFs for each channel, MHz.
  // TODO: make sure we're not off by a half-channel..
  for (int ichan=0; ichan<nchan; ichan++) {
    integration->set_centre_frequency(ichan, 
        get_centre_frequency() - 0.5*get_bandwidth() 
        + ichan*get_bandwidth()/(double)nchan);
  }

  // If the "no_amps" flag is set, the actual data is not called for, 
  // so we can exit early.  (Trying to actually load the data 
  // in this case will cause errors or segfaults.)
  if (Profile::no_amps) return integration;

  // Read in whole data array for this subint
  // Data structure according to http://www.naic.edu/~jeffh/wapp.html:
  //   Dimensions (fast->slow) = lags, bins, pols, subint
  // However, according to sigproc source (wapp_prof.c), it's actually:
  //   Dimensions (fast->slow) = lags, pols, bins, subint
  // The sigproc version seems to agree with reality...
  FILE *f = fopen(filename, "r");
  if (!f) 
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_Integration",
        "fopen(%s)", filename);
  int dump_size_floats = nchan*nbin*npol;
  size_t dump_size_bytes = 4*dump_size_floats;
  rv = fseek(f, wapp_ascii_hdr_size + wapp_hdr_size + subint*dump_size_bytes,
      SEEK_SET);
  if (rv<0) {
    fclose(f);
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_Integration",
        "fseek(subint %d)", subint);
  }
  float *data = new float[dump_size_floats];
  rv = fread(data, sizeof(float), dump_size_floats, f);
  fclose(f);
  if (rv!=dump_size_floats) 
    throw Error (FailedSys, "Pulsar::WAPPArchive::load_Integration",
        "fread(subint %d)", subint);

  // TODO : vanvleck correction
  // TODO : window?

  // FFT to get spectra from ACFs.  No real-to-real interface yet
  // so for now we'll do a r2c on a mirrored copy of the data.
  if (raw_data_is_lags) {
    float *mirror_data = new float[2*nchan];
    float *spec_data = new float[2*nchan+2];
    float *dptr;
    for (int ipol=0; ipol<npol; ipol++) {
      for (int ibin=0; ibin<nbin; ibin++) {
        dptr = &data[ibin*nchan*npol + ipol*nchan];
        mirror_data[0] = dptr[0];
        for (int ichan=1; ichan<nchan; ichan++) {
          mirror_data[ichan] = dptr[ichan];
          mirror_data[2*nchan-ichan] = dptr[ichan];
        }
        mirror_data[nchan]=0.0;
        FTransform::frc1d(2*nchan, spec_data, mirror_data);
        // Copy result back to data array.  Since input was symmetric
        // we only need to keep the real part.
        for (int ichan=0; ichan<nchan; ichan++) {
          dptr[ichan] = spec_data[2*ichan];
        }
      }
    }
    delete [] mirror_data;
    delete [] spec_data;
  }

  // Reorganize array, load into integration
  float *prof = new float[nbin];
  for (int ipol=0; ipol<npol; ipol++) {
    for (int ichan=0; ichan<nchan; ichan++) {
      for (int ibin=0; ibin<nbin; ibin++) {
        prof[ibin] = data[ibin*nchan*npol + ipol*nchan + ichan];
      }
      integration->get_Profile(ipol,ichan)->set_amps(prof);
    }
  }

  // Unallocate temp space
  delete [] prof;
  delete [] data;

  return integration;
}

void Pulsar::WAPPArchive::unload_file (const char* filename) const
{
  // unload all BasicArchive and WAPPArchive attributes as well as
  // BasicIntegration attributes and data to filename.

  // This does not need to be filled in if you never need to save files
  // in this data format.  Assuming this is the case, it's best to throw 
  // an error here:
  throw Error (InvalidState, "Pulsar::WAPPArchive::unload_file",
      "unload not implemented for WAPPArchive");

}

string Pulsar::WAPPArchive::Agent::get_description () 
{
  return "WAPP Archive Version 1.0";
}

bool Pulsar::WAPPArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by WAPP Archive, this method should return true.
  
  // One way to do this is to try to call the load_header method and
  // check for errors:
  try {
    WAPPArchive archive;
    archive.load_header(filename);
    return true;
  }
  catch (Error &e) {
    if (verbose > 2)
      cerr << "WAPP load failed due to: " << e << endl;
    return false;
  }

  return false;
}

