#ifndef __TOA_H
#define __TOA_H

#include <vector>
#include <string>

#include "MJD.h"

class archive;
class sub_int;
class profile;
class polyco;

class toa
{
 protected:

  enum Instrument { UNKNOWN, FPTM, FILTERBANK, S2, CPSR };
  enum Format { Unformatted, Princeton, Parkes, ITOA, Psrclock, Rhythm };

  Format format;

 public:
  // fundamental TOA LINE as on:
  // http://pulsar.princeton.edu/tempo/ref_man_sections/toa.txt
  double frequency;      // Observing frequency (MHz)
  MJD    arrival;        // TOA
  float  error;          // TOA uncertainty
  int    telescope;      // Observatory (one-character code)

  // Parkes Format specific
  float  phs;            // Phase offset (fraction of P0, added to TOA)
  // Princeton and ITOA Format specfic
  float  dmc;            // DM correction (pc cm^-3)
  // ITOA Format specific
  char   observatory[2]; // Observatory (two-letter code)

  // Rhythm extras
  time_t calculated;   /* the date when this toa was calculated */
  string filename;     /* name of archive file */
  archive* arch;       /* the archive from which this toa derives */
  int  subint;
  int  subband;
  int  subpoln;
  Instrument instrument;

  /* flags used when in group */
  static int verbose;
  char deleted;
  char selected;

  toa () { init(); };
  ~toa () { destroy(); };

  // copy constructor
  toa (const toa & in_toa);

  // construct from a profile and standard
  toa (const profile& pulsar_prf, const profile& std_prf,
       const MJD& prf_start_time, double folding_period, int nsite,
       const string& fname, int isubint, int isubband, int ipol);

  // construct from an open file
  toa (FILE* instream);

  // construct from a string
  toa (char* indata);

  // only operation
  double shift (const polyco & poly) const;

  // loading and unloading to/from file and string
  int    load   (FILE* instream);
  int    load   (const char* instring);
  int    unload (FILE* outstream, Format fmt = Unformatted) const;
  int    unload (char* outstring, Format fmt = Unformatted) const;

  int    parkes_parse (const char* instring);
  int    parkes_out   (char* outstring) const;
  int    Parkes_load      (const char* instring);
  int    Parkes_unload    (FILE* outstream) const;
  int    Parkes_unload    (char* outstring) const;

  int    Princeton_load   (const char* instring);
  int    Princeton_unload (FILE* outstream) const;
  int    Princeton_unload (char* outstring) const;

  int    Psrclock_load    (const char* instring);
  int    Psrclock_unload  (FILE* outstream) const;
  int    Psrclock_unload  (char* outstring) const;

  int    Rhythm_load      (const char* instring);
  int    Rhythm_unload    (FILE* outstream) const;
  int    Rhythm_unload    (char* outstring) const;

  int    Tempo_unload     (FILE* outstream) const;
  int    Tempo_unload     (char* outstring) const;

  // comparison operators
  friend int operator < (const toa& t1, const toa& t2)
    { return (t1.arrival < t2.arrival); };


  // operations on vectors of toas
  static int load (const char* filename, vector<toa>* toas);
  static int load (FILE* instream, vector<toa>* toas);
  
  static int unload (const char* filename, const vector<toa>& toas,
		     Format fmt = Unformatted);
  static int unload (FILE* outstream, const vector<toa>& toas,
		     Format fmt = Unformatted);
  
  static int mk_toas (const archive & pulsar_arch, const archive & std_arch,
		      vector<toa>* toas, int mode, int wt=0);

  static int mk_toas (const sub_int& pulsar_subint, const sub_int& std_subint,
		      int nsite, const string& fname, 
		      int subint, int nsubchan,
		      vector<toa>* toas, int mode, int wt=0);

 private:
  // low-level stuff
  void init();
  void destroy();
  void guess_instrument();
  bool valid();

  static void sizebuf (size_t length);
  static char*  buffer;
  static size_t bufsz;
  static char   datestr [25];
};

class toa_model {
 public:
  static int verbose;
  vector<toa> toas;

  toa_model(){};
  ~toa_model(){};
  toa_model (const toa_model & toamodel);
  toa_model(const archive & pulsar_arch, const archive & std_arch, int wt = 0);

  toa_model & operator=(const toa_model & toamodel);

  void load(const char * filename);
  void unload(const char * filename);
};

#endif
