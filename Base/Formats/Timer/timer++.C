#include <stdio.h>
#include <math.h>

#include "timer++.h"
#include "convert_endian.h"

int TIMERNAMESPACE timerload (const char* fname, struct timer* hdr, bool big_endian)
{
  FILE* fptr = fopen (fname, "r");
  if (fptr == NULL) {
    fprintf (stderr, "Timer::load Cannot open '%s'", fname);
    perror ("");
    return -1;
  }
  return timerload (fptr, hdr, big_endian);
}

int TIMERNAMESPACE timerload (FILE* fptr, struct timer* hdr, bool big_endian)
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
int TIMERNAMESPACE timerunload (FILE* fptr, struct timer& hdr)
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

MJD TIMERNAMESPACE timerget_MJD (const struct timer& hdr)
{ 
  return MJD(hdr.mjd, hdr.fracmjd);
}

void TIMERNAMESPACE timerset_MJD (struct timer& hdr, const MJD& mjd)
{
  hdr.mjd = mjd.intday();
  hdr.fracmjd = mjd.fracday();
}

int TIMERNAMESPACE timerpoln_storage (const struct timer& hdr)
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

bool TIMERNAMESPACE timermixable (const timer& hdr1, const timer& hdr2, double max_freq_sep)
{
  // Check headers - this is more or less a list of why we can't
  // store subints with different properties (e.g. nbin, sub_int_time...)

  if (timerpoln_storage (hdr1) != timerpoln_storage (hdr1)) {
    timerreason = "Archives have different polarization storage schemes";
    return false;
  }
  if (hdr1.obstype != hdr2.obstype ) {
    timerreason = "Archives have different observation type";
    return false;
  }
  if (strcmp (hdr1.psrname, hdr2.psrname)) {
    timerreason = "Archives arise from different pulsars";
    return false;
  }
  if (hdr1.nbin != hdr2.nbin) {
    timerreason = "Archives have different numbers of bins";
    return false;
  }
  if (hdr1.nsub_band != hdr2.nsub_band) {
    timerreason = "Archives have different numbers of subbands";
    return false;
  }
  if (fabs (hdr1.banda.centrefreq - hdr2.banda.centrefreq) > max_freq_sep) {
    timerreason = "Archives have too different center frequencies\n";
    return false;
  }
  if (hdr1.banda.correlator_mode != hdr2.banda.correlator_mode) {
    timerreason = "Archives have different correlator modes\n";
    return false;
  }
  if (hdr1.banda.bw != hdr2.banda.bw) {
    timerreason = "Archives have different bandwidths\n";
    return false;
  }
  if (hdr1.wtscheme != hdr2.wtscheme) {
    timerreason = "Archives have different weight schemes\n";
    return false;
  }

  // none of the above restrictions apply
  return true;
}
