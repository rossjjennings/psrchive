
#include "BasebandArchive.h"
#include "Error.h"

#include "machine_endian.h"
#include "compressed_io.h"
#include "timer++.h"


void Pulsar::BasebandArchive::init ()
{
  Timer::init (&hdr);
}

//! null constructor
Pulsar::BasebandArchive::BasebandArchive ()
{
  init ();
}

//! copy constructor
Pulsar::BasebandArchive::BasebandArchive (const Archive& archive)
{
  init ();
  BasebandArchive::copy (archive);
}

//! copy constructor
Pulsar::BasebandArchive::BasebandArchive (const BasebandArchive& archive)
{
  init ();
  BasebandArchive::copy (archive);
}

//! destructor
Pulsar::BasebandArchive::~BasebandArchive ()
{
  if (verbose)
    cerr << "BasebandArchive destructor" << endl;
}

//! Copy the contents of an Archive into self
void Pulsar::BasebandArchive::copy (const Archive& archive)
{
  if (verbose)
    cerr << "BasebandArchive::copy" << endl;

  if (this == &archive)
    return;

  TimerArchive::copy (archive);

  const BasebandArchive* barchive;
  barchive = dynamic_cast<const BasebandArchive*>(&archive);
  if (!barchive)
    return;

  if (verbose)
    cerr << "BasebandArchive::copy another BasebandArchive" << endl;

  bhdr = barchive->bhdr;
  dls_histogram = barchive->dls_histogram;
  passband = barchive->passband;
}

//! Returns a pointer to a new copy of self
Pulsar::Archive* Pulsar::BasebandArchive::clone () const
{
  return new BasebandArchive (*this);
}


// /////////////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////////////
//
// /////////////////////////////////////////////////////////////////////////


//! Register the BasebandArchive Agent
static Registry::List<Pulsar::Archive::Agent>::Enter<Pulsar::BasebandAgent> e;

Pulsar::BasebandAgent::BasebandAgent ()
  : Pulsar::Archive::Agent ("TIMER Archive with Baseband Extensions version 4")
{
}

//! return true if filename refers to a timer archive
bool Pulsar::BasebandAgent::advocate (const char* filename)
{
  struct timer hdr;
  if ( Timer::fload (filename, &hdr, TimerArchive::big_endian) < 0 )
    return false;

  if ( strncmp (hdr.backend, "baseband", BACKEND_STRLEN) != 0 )
    return false;

  return true;
}

Pulsar::Archive* Pulsar::BasebandAgent::new_Archive () 
{
  return new BasebandArchive;
}

// //////////////////////////////////////////////////////////////////////
// Size of the baseband_header struct as a function of Version number
static unsigned header_sizes [BASEBAND_HEADER_VERSION + 1] =
{ 104,   // Version 0
  112,   // Version 1
  112,   // Version 2
  128,   // Version 3
  176    // Version 4
};

// //////////////////////////////////////////////////////////////////////
// FLAGS
#define DynamicLevels  0x0000000000000001
// 3 unused bits

// //////////////////////////////////////////////////////////////////////
// pre-detection data windowing in the time domain
#define TimeHanning    0x0000000000000010
#define TimeWelch      0x0000000000000020
#define TimeParzen     0x0000000000000040

// //////////////////////////////////////////////////////////////////////
// how the data was detected
#define Stokes         0x0000000000000100
#define Britton        0x0000000000000200

// //////////////////////////////////////////////////////////////////////
// post-detection smoothing in the time domain
#define DetectHanning  0x0000000000001000  // Hanning smoothing


// //////////////////////////////////////////////////////////////////////
// how data was manipulated in frequency space
#define DeDispersion   0x0000000000000001  // coherent de-dispersion
#define FbankDedisp    0x0000000000000002  // kernel applied to subchannels
#define CorInstDepol   0x0000000000000004  // correct instrumental depol

#define ScatteredPower 0x0000000000000010  // scattered power correction
#define IntegratePband 0x0000000000001000  // integrated pass-band
#define PbandIntensity 0x0000000000010000  // pass-band is intensity


void Pulsar::BasebandArchive::convert_hdr_Endian ()
{
  ChangeEndian (bhdr.endian);
  ChangeEndian (bhdr.version);
  ChangeEndian (bhdr.size);
  
  ChangeEndian (bhdr.totaljobs);
  ChangeEndian (bhdr.gulpsize);
  ChangeEndian (bhdr.seeksize);
  
  ChangeEndian (bhdr.voltage_state);
  ChangeEndian (bhdr.analog_channels);
  ChangeEndian (bhdr.ppweight);
  ChangeEndian (bhdr.dls_threshold_limit);
  
  ChangeEndian (bhdr.pband_channels);
  ChangeEndian (bhdr.pband_resolution);
  ChangeEndian (bhdr.f_resolution);
  ChangeEndian (bhdr.t_resolution);
  
  ChangeEndian (bhdr.nfft);
  ChangeEndian (bhdr.nsmear);
  
  ChangeEndian (bhdr.nscrunch);
  ChangeEndian (bhdr.power_normalization);

  ChangeEndian (bhdr.time_domain);
  ChangeEndian (bhdr.frequency_domain);
  ChangeEndian (bhdr.mean_power_cutoff);    // Version 1 addition
  ChangeEndian (bhdr.hanning_smoothing);    // Version 2 addition  
}

// written for the sake of fixing a limited set of broken files
// that first came out on monolith
static bool uint64_bug = false;

void Pulsar::BasebandArchive::backend_load (FILE* fptr)
{
  bool swap_endian = false;

  long file_start = ftell (fptr);

  if (fread (&bhdr, sizeof(baseband_header), 1, fptr) < 1)
    throw Error (FailedSys, "BasebandArchive::backend_load",
		 "fread baseband_header");

  if (bhdr.endian == BASEBAND_OPPOSITE_ENDIAN) {
    if (verbose)
      cerr << "BasebandArchive::backend_load opposite_endian" << endl;
    convert_hdr_Endian();
    swap_endian = true;
  }

  if (bhdr.endian != BASEBAND_HEADER_ENDIAN)
    throw Error (InvalidState, "BasebandArchive::backend_load",
		 "invalid endian code: %x", bhdr.endian);

  int header_size_diff = 0;

  if (bhdr.version != BASEBAND_HEADER_VERSION)
    header_size_diff = BASEBAND_HEADER_SIZE - header_sizes[bhdr.version];

  if (header_size_diff) {
    // correct the FILE* for differing Version header size
    if (fseek(fptr, -header_size_diff, SEEK_CUR) < 0)
      throw Error (FailedSys, "BasebandArchive::backend_load", 
		   "fseek(%ld)\n", -header_size_diff);
  }

  // ////////////////////////////////////////////////////////////////////////
  //
  // Version corrections performed here must be done in incremental order
  //
  // ////////////////////////////////////////////////////////////////////////

  bool swap_passband = false;

  if (bhdr.version < 1) {
    bhdr.mean_power_cutoff = 0.0;

    if (hdr.nsub_band == 1 && bhdr.voltage_state == 2)
      swap_passband = true;

    bhdr.version = 1;
  }

  if (bhdr.version < 2) {
    if (bhdr.t_resolution == 2)
      set_hanning_smoothing_factor (2);
    else
      bhdr.hanning_smoothing = 0;
    bhdr.version = 2;
  }

  // ////////////////////////////////////////////////////////////////////////
  // end Version corrections
  // ////////////////////////////////////////////////////////////////////////

  if (bhdr.ppweight) {

    if (verbose) cerr << "BasebandArchive::backend_load " 
		      << bhdr.analog_channels << " histograms.\n";

    dls_histogram.resize (bhdr.analog_channels);

    for (int idc=0; idc<bhdr.analog_channels; idc++)
      if (fread_compressed (fptr, &dls_histogram[idc], swap_endian) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_load",
		     "fread_compressed histogram[%d]", idc);

  }

  if (bhdr.pband_resolution) {

    if (verbose) cerr << "BasebandArchive::backend_load "
		      << bhdr.pband_channels << " bandpasses.\n";

    passband.resize (bhdr.pband_channels);

    for (int ipb=0; ipb<bhdr.pband_channels; ipb++)
      if (fread_compressed (fptr, &passband[ipb], swap_endian) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_load",
		     "fread_compressed passband[%d]", ipb);

  }

  if (swap_passband) {
    int nswap = bhdr.pband_resolution / 2;
 
    for (int ipb=0; ipb<bhdr.pband_channels; ipb++)
      for (int ipt=0; ipt<nswap; ipt++)
	swap (passband[ipb][ipt], passband[ipb][nswap+ipt]);
  }


  long file_end = ftell (fptr);

  if ((file_end - file_start) != (long)hdr.be_data_size)
    throw Error (InvalidState, "BasebandArchive::backend_load",
		 "loaded %d bytes. advertised %d bytes",
		 file_end - file_start, hdr.be_data_size);

  if (uint64_bug) {
    hdr.be_data_size += (bhdr.pband_channels + bhdr.analog_channels) * 4;
    bhdr.size = hdr.be_data_size;
  }

  // correct the timer header in preparation for unloading this archive
  bhdr.size = hdr.be_data_size += header_size_diff;

}

void Pulsar::BasebandArchive::backend_unload (FILE* fptr) const
{
  if (bhdr.size != hdr.be_data_size)
    throw Error (InvalidState, "BasebandArchive::backend_unload",
		 "invalid header size (timer:%d, bhdr:%d)\n", 
		 hdr.be_data_size, bhdr.size);
  
  long file_start = ftell (fptr);

  if (fwrite (&bhdr, sizeof(baseband_header), 1, fptr) < 1)
    throw Error (FailedSys, "BasebandArchive::backend_unload",
		 "fwrite baseband_header");

  long file_end = ftell (fptr);

  if (file_end - file_start != BASEBAND_HEADER_SIZE)
    throw Error (InvalidState, "BasebandArchive::backend_unload",
		 "unloaded %d bytes of BASEBAND header (size=%d)\n",
		 file_end - file_start, BASEBAND_HEADER_SIZE);


  if (bhdr.ppweight) {

    for (int idc=0; idc<bhdr.analog_channels; idc++)
      if (fwrite_compressed (fptr, dls_histogram[idc]) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_unload",
		     "fwrite_compressed histogram[%d]", idc);

  }

  if (bhdr.pband_resolution) {

    for (int ipb=0; ipb<bhdr.pband_channels; ipb++)
      if (fwrite_compressed (fptr, passband[ipb]) < 0)
	throw Error (FailedSys, "BasebandArchive::backend_unload",
		     "fwrite_compressed passband[%d]", ipb);

  }

  file_end = ftell (fptr);

  if ((file_end - file_start) != (long) hdr.be_data_size)
    throw Error (InvalidState, "BasebandArchive::backend_unload",
		 "unloaded %d bytes. advertised %d bytes\n",
		 file_end - file_start, hdr.be_data_size);
}

vector<float> Pulsar::BasebandArchive::empty (0);

const vector<float>& Pulsar::BasebandArchive::get_passband (unsigned channel) const
{
  if (channel >= passband.size ())
    return empty;
  return passband[channel];
}

const vector<float>& Pulsar::BasebandArchive::get_histogram (unsigned channel) const
{
  if (channel >= dls_histogram.size ())
    return empty;
  return dls_histogram[channel];
}

bool Pulsar::BasebandArchive::get_scattered_power_corrected () const {
  return bhdr.frequency_domain & ScatteredPower;
}

bool Pulsar::BasebandArchive::get_coherent_calibration () const {
  return bhdr.frequency_domain & CorInstDepol;
}

void Pulsar::BasebandArchive::set_hanning_smoothing_factor (int factor)
{
  bhdr.hanning_smoothing = factor;    // Version 2 addition
  if (factor < 2)
    bhdr.time_domain &= ~DetectHanning;
  else
    bhdr.time_domain |= DetectHanning;
}

int Pulsar::BasebandArchive::get_hanning_smoothing_factor () const
{
  if ((bhdr.time_domain & DetectHanning) == 0)
    return 0;
  else
    return bhdr.hanning_smoothing;
}

float Pulsar::BasebandArchive::get_tolerance () const
{
  return bhdr.mean_power_cutoff;
}

const char* Pulsar::BasebandArchive::get_apodizing_name () const
{
  if (bhdr.time_domain & TimeHanning)
    return "Hanning";
  else if (bhdr.time_domain & TimeParzen)
    return "Parzen";
  else if (bhdr.time_domain & TimeWelch)
    return "Welch";
  else
    return "None";
}
