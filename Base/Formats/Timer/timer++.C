#include <stdio.h>
#include <math.h>

#include "timer++.h"
#include "convert_endian.h"

string Timer::reason;
bool   Timer::verbose = false;

unsigned long Timer::backend_data_size (const struct timer& hdr)
{
  if (backend_recognized (hdr.backend) != -1)
    return hdr.be_data_size;
  return 0;
}

int Timer::backend_recognized (const char* backend)
{
  for (int ibe=0; ibe<nbackends; ibe++)
    if (strncmp (backends[ibe], backend, BACKEND_STRLEN) == 0)
      return ibe;
  return -1;
}

int Timer::set_backend (struct timer* hdr, const char* backend)
{
  if (backend_recognized (backend) == -1)
    return -1;

  memcpy (hdr->backend, backend, BACKEND_STRLEN);
  return 0;
}

string Timer::get_backend (const struct timer& hdr)
{
  int ibe = backend_recognized (hdr.backend);
  if (ibe < 0)
    return string ("un-recognized");

  return string (backends[ibe]);
}

int Timer::fload (const char* fname, struct timer* hdr, bool big_endian)
{
  FILE* fptr = fopen (fname, "r");
  if (fptr == NULL) {
    if (verbose) {
      fprintf (stderr, "Timer::load Cannot open '%s'", fname);
      perror ("");
    }
    return -1;
  }
  int ret = load (fptr, hdr, big_endian);
  fclose (fptr);
  return ret;
}

int Timer::load (FILE* fptr, struct timer* hdr, bool big_endian)
{
  if (fread (hdr, sizeof(struct timer), 1, fptr) < 1)  {
    if (verbose) {
      fprintf (stderr, "Timer::load Cannot read timer struct from FILE*");
      perror ("");
    }
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
    if (verbose) {
      fprintf(stderr,"Timer::unload Cannot write timer struct to FILE*");
      perror ("");
    }
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
//string Timer::reason;

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

void band_init (struct band * bd)
{
  bd->lo1 = -1.0;
  bd->lo2 = -1.0;
  bd->loUP = -1.0;
  bd->loDOWN = -1.0;
  bd->centrefreq = -1.0;
  bd->bw = -1.0;
  bd->flux_A = -1.0;
  bd->inv_mode = -1;
  bd->auto_atten = 0;
  bd->correlator_mode = -1;
  bd->f_atten_A = -1.0;
  bd->f_atten_B = -1.0;
  bd->polar = -1;
  bd->feed_offset = -1.0;
  bd->nlag = -1;
  bd->flux_B = -1.0;
  bd->flux_err = -1.0;
  bd->npol = -1;
}

void Timer::init (struct timer * hdr)
{
  band_init (&(hdr->banda));
  band_init (&(hdr->bandb));

  /* nothing yet */
  strcpy(hdr->ram_boards," ");
  strcpy(hdr->corr_boards," ");
  strcpy(hdr->machine_id," ");
  strcpy(hdr->tpover,"10.0");
  hdr->tape_number=-1;
  hdr->file_number=-1;
  strcpy(hdr->utdate,"dd-mm-yyyy");
  hdr->feedmode=0;
  hdr->fracmjd=-1.0;
  hdr->lst_start=-99999999.0;
  hdr->mjd=-1;
  hdr->number_of_ticks=0;
  hdr->offset=0.0;
  strcpy(hdr->psrname,"Null");
  hdr->ra=-1000.0;
  hdr->dec=-1000.0;
  hdr->l=-1000.0;
  hdr->b=-1000.0;
  hdr->nominal_period=-1.0;
  hdr->dm=0;
  hdr->sub_int_time=0.0;
  hdr->pos_angle=-10000.0;        
  hdr->nbin=0;
  hdr->tsmp=-1.0;
  hdr->narchive_int=-1;
  hdr->nsub_int=-1;
  hdr->dump_time=0.0;
  hdr->wts_and_bpass= 0;
  hdr->wtscheme=0;
  strcpy(hdr->coord_type,"05"); /* J2000 */
  hdr->nominal_period=-1.0;
  hdr->fold_true_ratio=1;
  hdr->nperiods_long = -1;  /* periods per profile */
  hdr->nperiods_short= 0;   /* periods in dead time */
  hdr->ndump_sub_int=0;
  hdr->junk=0;
  hdr->junk2=0;
  hdr->nfreq=1;
  hdr->nspan=0;
  hdr->ncoeff=0;
  hdr->rotm=-100000.0; /* catalogue */
  hdr->pnterr=-100000.0;
  hdr->ibeam=0;
  strcpy(hdr->tape_label," ");	 	
  hdr->nsub_band=-1;
  strcpy(hdr->tree," ");			
  strcpy(hdr->telid," ");			
  hdr->nbytespoly=0;				
  hdr->nbytesephem=0;				
  hdr->headerlength=1024;			
  strcpy(hdr->calfile," ");			
  strcpy(hdr->scalfile," ");			
  strcpy(hdr->comment," ");			
  strcpy(hdr->schedule," ");			
  strcpy(hdr->software,"unknown");			
  strcpy(hdr->backend,"unknown");			
  hdr->be_data_size=0;
  hdr->corrected=0;
  hdr->calibrated=0;				
  hdr->obstype=-1;
}

