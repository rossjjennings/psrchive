#include <stdio.h>
#include <math.h>

#include "timer++.h"
#include "convert_endian.h"

int Timer::load (const char* fname, struct timer* hdr, bool big_endian)
{
  FILE* fptr = fopen (fname, "r");
  if (fptr == NULL) {
    fprintf (stderr, "Timer::load Cannot open '%s'", fname);
    perror ("");
    return -1;
  }
  return load (fptr, hdr, big_endian);
}

int Timer::load (FILE* fptr, struct timer* hdr, bool big_endian)
{
  if (fread (hdr, sizeof(struct timer), 1, fptr) < 1)  {
    fprintf (stderr, "Timer::load Cannot read timer struct from FILE*");
    perror ("");
    return -1;
  }
  if (big_endian)
    timer_fromBigEndian (hdr);
  else
    timer_fromLittleEndian (hdr);
  return 0;
}

// unloads a timer struct to a file (always big endian)
int Timer::unload (FILE* fptr, struct timer& hdr)
{
  timer_toBigEndian (&hdr);
  if (fwrite (&hdr, sizeof(struct timer), 1, fptr) < 1)  {
    fprintf(stderr,"Timer::unload Cannot write timer struct to FILE*");
    perror ("");
    return -1;
  }
  timer_fromBigEndian(&hdr);
  return 0;
}

MJD Timer::get_MJD (const struct timer& hdr)
{ 
  return MJD(hdr.mjd, hdr.fracmjd);
}

void Timer::set_MJD (struct timer& hdr, const MJD& mjd)
{
  hdr.mjd = mjd.intday();
  hdr.fracmjd = mjd.fracday();
}

int Timer::poln_storage (const struct timer& hdr)
{
  string mch = hdr.machine_id;
  if (mch=="S2" || mch=="CPSR") 
    return IQ_POLN_STORAGE;
  else 
    return XY_POLN_STORAGE;
}

// ///////////////////////////////////////////////////////////////////////
// Timer::mixable
//
// returns true if the conditions are met that allow two archives to be
// mixed together.  Otherwise returns false.
//
// ///////////////////////////////////////////////////////////////////////

// the difference in frequencies should not be more than 100kHz...
// double Timer::default_max_freq_sep = 0.1;
// string Timer::reason;

bool Timer::mixable (const timer& hdr1, const timer& hdr2, double max_freq_sep)
{
  // Check headers - this is more or less a list of why we can't
  // store subints with different properties (e.g. nbin, sub_int_time...)

  if (poln_storage (hdr1) != poln_storage (hdr1)) {
    reason = "Archives have different polarization storage schemes";
    return false;
  }
  if (hdr1.obstype != hdr2.obstype ) {
    reason = "Archives have different observation type";
    return false;
  }
  if (strcmp (hdr1.psrname, hdr2.psrname)) {
    reason = "Archives arise from different pulsars";
    return false;
  }
  if (hdr1.nbin != hdr2.nbin) {
    reason = "Archives have different numbers of bins";
    return false;
  }
  if (hdr1.nsub_band != hdr2.nsub_band) {
    reason = "Archives have different numbers of subbands";
    return false;
  }
  if (fabs (hdr1.banda.centrefreq - hdr2.banda.centrefreq) > max_freq_sep) {
    reason = "Archives have too different center frequencies\n";
    return false;
  }
  if (hdr1.banda.correlator_mode != hdr2.banda.correlator_mode) {
    reason = "Archives have different correlator modes\n";
    return false;
  }
  if (hdr1.banda.bw != hdr2.banda.bw) {
    reason = "Archives have different bandwidths\n";
    return false;
  }
  if (hdr1.wtscheme != hdr2.wtscheme) {
    reason = "Archives have different weight schemes\n";
    return false;
  }

  // none of the above restrictions apply
  return true;
}
