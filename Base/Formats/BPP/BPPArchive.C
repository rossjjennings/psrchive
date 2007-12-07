/***************************************************************************
 *
 *   Copyright (C) 2007 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/BPPArchive.h"
#include "Pulsar/BasicIntegration.h"
#include "Pulsar/Profile.h"

#include "machine_endian.h"

using namespace std;

void Pulsar::BPPArchive::init ()
{
  // I think only BPP-specifc init needs to 
  // go in here.
  orig_rfs = NULL;
  orig_rfs_corrected = 0;
}

Pulsar::BPPArchive::BPPArchive()
{
  init ();
}

Pulsar::BPPArchive::~BPPArchive()
{
  // destroy any BPPArchive resources
  // Again, I think this is only for BPP-specific stuff.
  if (orig_rfs!=NULL) delete [] orig_rfs;
}

Pulsar::BPPArchive::BPPArchive (const Archive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive construct copy Archive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::BPPArchive::BPPArchive (const BPPArchive& arch)
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive construct copy BPPArchive" << endl;

  init ();
  Archive::copy (arch);
}

Pulsar::BPPArchive::BPPArchive (const Archive& arch, 
                                const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive construct extract Archive" << endl;

  init ();
  Archive::copy (arch, subints);
}


void Pulsar::BPPArchive::copy (const Archive& archive, 
                                   const vector<unsigned>& subints)
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::copy" << endl;

  if (this == &archive)
    return;

  Archive::copy (archive, subints);

  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::copy dynamic cast call" << endl;
  
  const BPPArchive* like_me = dynamic_cast<const BPPArchive*>(&archive);
  if (!like_me)
    return;
  
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::copy another BPPArchive" << endl;

  // copy BPPArchive-specific attributes
}

Pulsar::BPPArchive* Pulsar::BPPArchive::clone () const
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::clone" << endl;
  return new BPPArchive (*this);
}

Pulsar::BPPArchive* 
Pulsar::BPPArchive::extract (const vector<unsigned>& subints) const
{
  if (verbose > 2)
    cerr << "Pulsar::BPPArchive::extract" << endl;
  return new BPPArchive (*this, subints);
}

/* Actual data reading code below this point.
 * Structure of a raw BPP file is as follows:
 *
 * Overall file:
 *   1 x struct bpp_header
 *   hdr.bds x (rf_array mean_array data_array)
 *
 * rf_array:
 *   hdr.chsbd x double - RF(Hz) for each chan.
 * mean_array:
 *   hdr.chsbd*hdr.poln x float - Data offset for each chan,pol.
 * data_array:
 *   hdr.chsdb*hdr.poln*hdr.bins x float - Actual data for bin,chan,pol.
 *
 * Quirks:
 *   - The data in rf_array is not quite accurate, and needs to be corrected
 *     via the function fix_orig_rfs.
 *   - The endianess of the file agrees with PCs (little-endian).
 *   - Later files have 3 groups of 8 channels, where the middle group
 *     overlaps the frequency range of the outer 2.  In the end, we probably
 *     want to ignore this middle group (TODO)...
 */

void Pulsar::BPPArchive::fix_orig_rfs ()
{
  // Don't double-fix
  if (orig_rfs_corrected) return;
  
  // Error if rfs not loaded.  We'll assume that the
  // header is loaded if the RF array is present.
  if (orig_rfs==NULL) 
    throw Error (InvalidState, "Pulsar::BPPArchive::fix_orig_rfs", 
        "RFs not loaded yet");

  // Don't know what to do if nchan is not a multiple of 8
  int nchan_tmp = hdr.bds*hdr.chsbd;
  if (nchan_tmp % 8) 
    throw Error (InvalidState, "Pulsar::BPPArchive::fix_orig_rfs",
        "nchan not a multiple of 8 (nchan=%d)", nchan_tmp);

  // Fix RF values.  We'll only use info from the hdr struct.
  double rf0;
  int i, j;
  for (i=0; i<nchan_tmp; i+=8) {
    rf0 = 0.5*(orig_rfs[i+3] + orig_rfs[i+4]);
    for (j=0; j<8; j++) {
      orig_rfs[i+j] = rf0 + ((double)j-3.5)*hdr.bandwidth;
    }
  }

  // Update flag
  orig_rfs_corrected=1;
}

void Pulsar::BPPArchive::hdr_to_big_endian () 
{
#if (MACHINE_LITTE_ENDIAN==0) // Compile-time endian check
  if (verbose>2)
    cerr << "Pulsar::BPPArchive::hdr_to_big_endian" << endl;
  ChangeEndian(hdr.telescope);
  ChangeEndian(hdr.apparent_period);
  ChangeEndian(hdr.dispersion_measure);
  ChangeEndian(hdr.scan_number);
  ChangeEndian(hdr.crate_id);
  ChangeEndian(hdr.year);
  ChangeEndian(hdr.day);
  ChangeEndian(hdr.seconds);
  ChangeEndian(hdr.second_fraction);
  ChangeEndian(hdr.RF_of_chan_0);
  ChangeEndian(hdr.bandwidth);
  ChangeEndian(hdr.integration_time);
  ChangeEndian(hdr.bins);
  ChangeEndian(hdr.bds);
  ChangeEndian(hdr.chsbd);
  ChangeEndian(hdr.polns);
  ChangeEndian(hdr.IF0_gain);
  ChangeEndian(hdr.IF1_gain);
  ChangeEndian(hdr.IF2_gain);
  ChangeEndian(hdr.IF3_gain);
  ChangeEndian(hdr.RF_of_IFcenter_0);
  ChangeEndian(hdr.RF_of_IFcenter_1);
  ChangeEndian(hdr.RF_of_IFcenter_2);
  ChangeEndian(hdr.RF_of_IFcenter_3);
  ChangeEndian(hdr.SRAM_base_freq);
  ChangeEndian(hdr.SRAM_freq_incr);
  ChangeEndian(hdr.bytes);
#endif
}

void Pulsar::BPPArchive::load_header (const char* filename)
{
  // load all BasicArchive and BPPArchive attributes from filename
  // We need to set (at least?) nbin, nchan, npol, nsubint

  // Open file, read header struct
  FILE *f = fopen(filename,"r");
  if (!f) 
    throw Error (FailedSys, "Pulsar::BPPArchive::load_header",
        "fopen(%s)", filename);

  int rv = fread(&hdr, sizeof(bpp_header), 1, f);

  if (rv!=1) {
    fclose(f);
    throw Error (FailedCall, "Pulsar::BPPArchive::load_header", "fread");
  }

#if (MACHINE_LITTLE_ENDIAN==0) 
  // Byte-swap header if needed
  hdr_to_big_endian();
#endif
  
  // Read in RFs array now to make life easier
  int i;
  orig_rfs = new double[hdr.chsbd*hdr.bds];
  int means_array_size = sizeof(float)*hdr.chsbd*hdr.polns;
  int data_array_size = sizeof(float)*hdr.bins*hdr.chsbd*hdr.polns;
  for (i=0; i<hdr.bds; i++) {
    rv = fread(&orig_rfs[i*hdr.chsbd], sizeof(double), hdr.chsbd, f);
    if (rv!=hdr.chsbd) { 
      fclose(f);
      throw Error (FailedCall, "Pulsar::BPPArchive::load_header", 
          "fread(orig_rfs)");
    }
    rv = fseek(f, means_array_size + data_array_size, SEEK_CUR);
    if (rv) { 
      fclose(f);
      throw Error (FailedCall, "Pulsar::BPPArchive::load_header", 
          "fseek(orig_rfs)");
    }
  }
  fclose(f);

#if (MACHINE_LITTLE_ENDIAN==0)
  // Byte-swap RFs array if needed
  array_changeEndian(hdr.chsbd*hdr.bds, orig_rfs, sizeof(double));
#endif

  // Check for expected version number
  if (strncmp(hdr.version,"1.0",3)==0) { rv=1; }
  else if (strncmp(hdr.version,"2.0",3)==0) { rv=1; }
  else { rv=0; }

  if (!rv)
    throw Error (InvalidState, "Pulsar::BPPArchive::load_header",
        "version not recognized");


  // Convert raw header values to things PSRCHIVE wants
  set_npol(hdr.polns);
  if (npol==1) { set_state(Signal::Intensity); }
  else if (npol==2) { set_state(Signal::PPQQ); }
  else if (npol==4) { set_state(Signal::Coherence); }
  else
    throw Error (InvalidState, "Pulsar::BPPArchive::load_header",
        "invalid npol (%d)", npol);

  set_nsubint(1); // Only 1 subint per bpp file ever
  set_nbin(hdr.bins);
  set_nchan(hdr.chsbd*hdr.bds); // chans/board * n_boards

  set_scale(Signal::FluxDensity);

  string src = hdr.pulsar_name;
  set_source(src);

  char site_tmp[2];
  site_tmp[0] = (char)hdr.telescope;
  site_tmp[1] = '\0';
  set_telescope(site_tmp);

  if (src[0]=='c' || src[0]=='C') {
    set_type(Signal::PolnCal);
  } else if (src[0]=='d' || src[0]=='D') {
    if (src[src.length()-1]=='n' || src[src.length()-1]=='N') { 
      set_type(Signal::FluxCalOff); 
    } else { set_type(Signal::FluxCalOn); }
  } else {
    set_type(Signal::Pulsar);
  }

  // Find total (non-overlapping) BW
  // BPP file convention: bw is always positive, rf<0 for reversed sideband.
  // PSRCHIVE freq convention is opposite (rf>0 always, bw<0 for reverse).
  // Units: BPP=Hz, PSRCHIVE=MHz
  double rf_min, rf_max, rf_avg=0.0;
  fix_orig_rfs();
  rf_min = orig_rfs[0];
  rf_max = orig_rfs[0];
  for (i=0; i<nchan; i++) { 
    rf_avg += orig_rfs[i];
    if (orig_rfs[i]<rf_min) rf_min = orig_rfs[i];
    if (orig_rfs[i]>rf_max) rf_max = orig_rfs[i];
  }
  rf_min = fabs(rf_min-hdr.bandwidth/2.0);
  rf_max = fabs(rf_max+hdr.bandwidth/2.0);
  set_bandwidth((rf_max-rf_min)/1e6);
  rf_avg /= (double)nchan;
  rf_avg = fabs(rf_avg);
  set_centre_frequency(rf_avg/1e6);

  // Alt version, this is probably a more accurate representation
  // of the total BW.  Depends on what one means by bandwidth in
  // the case where channels can either overlap or have small
  // gaps between them...
  set_bandwidth((double)nchan*fabs(hdr.bandwidth)/1e6);

  set_dispersion_measure(hdr.dispersion_measure);
  set_dedispersed(false);
  set_faraday_corrected(false);
  set_poln_calibrated(false);

  // Why are there no time values loaded in here?

}

int Pulsar::BPPArchive::get_mjd_from_hdr() 
{
  // Date is stored as year, day of year in the header.
  // Maybe we can add a constructor for this date type
  // to the MJD class at some point?
  const int mjd_1990_01_01 = 47892; // BPP data starts in 1994
  if (hdr.year<1990)
    throw Error (InvalidState, "Pulsar::BPPArchive::get_mjd_from_hdr",
        "year<1990 not supported (%d)", hdr.year);
  int sum = hdr.day-1; // Jan 1 = day 1
  for (int i=1990; i<hdr.year; i++) {
    sum += 365;
    if ((i%4)==0) { sum++; } // not y2.1k compliant ;)
  }
  return(mjd_1990_01_01 + sum);
}

// TODO Implement this
Pulsar::Integration*
Pulsar::BPPArchive::load_Integration (const char* filename, unsigned subint)
{

  // load all BasicIntegration attributes and data from filename

  // Can we assume header has been loaded?  Check for presence
  // of RFs array.  If not there, should we call load_header here?
  if (orig_rfs==NULL) 
    throw Error (InvalidState, "Pulsar::BPPArchive::load_Integration", 
        "RFs not loaded yet");

  // There is only 1 subint per BPP file
  if (subint!=0) 
    throw Error (InvalidState, "Pulsar::BPPArchive::load_Integration",
        "Requested subint!=0 (%d)", subint);

  // Size of various data sections per board
  const size_t size_hdr = sizeof(struct bpp_header);
  int num_rfs = hdr.chsbd;
  int num_means = num_rfs * hdr.polns;
  //int num_data = num_means * hdr.bins;
  size_t size_rfs = num_rfs * sizeof(double);
  //size_t size_means = num_means * sizeof(float);
  //size_t size_data = num_data * sizeof(float);

  Pulsar::BasicIntegration* integration = new BasicIntegration;
  resize_Integration(integration); // Sizes integration to npol,nchan,nbin

  // Date/time stuff 
  MJD epoch(get_mjd_from_hdr(), hdr.seconds, hdr.second_fraction);
  integration->set_epoch(epoch);
  integration->set_duration(hdr.integration_time);
  integration->set_folding_period(hdr.apparent_period);

  // Set center freqs
  for (int i=0; i<hdr.bds*hdr.chsbd; i++) {
      integration->set_centre_frequency(i, fabs(orig_rfs[i])/1e6);
  }

  // Set uniform weights
  integration->uniform_weight(1.0);

  // If no_amps is set, we don't need to read the actual data
  if (Profile::no_amps) return integration;

  // Open up file
  FILE *f = fopen(filename, "r");
  if (!f) 
    throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration",
        "fopen(%s)", filename);

  // Loop over boards, channels
  Pulsar::Profile *prof;
  int cur_chan;
  float *data = new float[hdr.bins];
  float *means = new float[num_means];
  int rv = fseek(f, size_hdr, SEEK_SET); // Already read this
  if (rv) 
    throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration", "fseek");
  for (int i=0; i<hdr.bds; i++) {
    rv = fseek(f, size_rfs, SEEK_CUR); // Already read these
    if (rv) 
      throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration", "fseek");
    rv = fread(means, sizeof(float), num_means, f);
    if (rv!=num_means)
      throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration", 
          "fread(means) rv=%d", rv);
#if (MACHINE_LITTLE_ENDIAN==0)
    // Byte-swap means array if needed
    array_changeEndian(num_means, means, sizeof(float));
#endif
    for (int j=0; j<hdr.polns; j++) {
      for (int k=0; k<hdr.chsbd; k++) {
        cur_chan = i*hdr.chsbd + k;
        rv = fread(data, sizeof(float), hdr.bins, f);
        if (rv!=hdr.bins)
          throw Error (FailedSys, "Pulsar::BPPArchive::load_Integration",
              "fread(data) rv=%d", rv);
#if (MACHINE_LITTLE_ENDIAN==0)
        // Byte-swap data
        array_changeEndian(hdr.bins, data, sizeof(float));
#endif
        prof = integration->get_Profile(j,cur_chan);
        prof->set_amps(data);
        prof->offset(means[j*hdr.chsbd+k]);
      }
    }
  }
  fclose(f);

  // Unalloc temp space
  delete [] data;
  delete [] means;

  return integration;
}

void Pulsar::BPPArchive::unload_file (const char* filename) const
{
  // unload is not implemented
  throw Error (InvalidState, "Pulsar::BPPArchive::unload_file",
          "not implemented");
}



string Pulsar::BPPArchive::Agent::get_description () 
{
  return "BPP Archive Version 0.1";
}

bool Pulsar::BPPArchive::Agent::advocate (const char* filename)
{
  // if the file named by filename contains data in the format recognized
  // by BPP Archive, this method should return true
  // Implement by trying to load header
  try {
    BPPArchive archive;
    archive.load_header (filename);
    return true;
  }
  catch (Error &e) { 
    return false;
  }
  return false;
}

