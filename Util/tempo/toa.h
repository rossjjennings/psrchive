#ifndef __TOA_H
#define __TOA_H

#include <vector>
#include <string>

#include "MJD.h"

class polyco;

class toa
{
 public:

  enum Instrument { UNKNOWN, FPTM, FILTERBANK, S2, CPSR };
  enum Format { Unspecified, Princeton, Parkes, ITOA, Psrclock, Rhythm };

 protected:
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
  int  subint;
  int  subband;
  int  subpoln;
  Instrument instrument;

  Format format;

  /* flags used when in group */
  char deleted;
  char selected;

 public:
  static int verbose;

  toa () { init(); };
  ~toa () { destroy(); };

  // copy constructor
  toa (const toa & in_toa);

  // construct from an open file
  toa (FILE* instream);

  // construct from a string
  toa (char* indata);

  // methods for setting/getting things (may eventually check validity)
  void set_instrument(Instrument i){ instrument = i; };
  void set_format    (Format fmt)  { format = fmt; };
  void set_frequency (double freq) { frequency = freq; };
  void set_arrival   (MJD arrived) { arrival = arrived; };
  void set_error     (float err)   { error = err; };
  void set_telescope (int telcode) { telescope = telcode; };
  void set_when_calculated (time_t when) { calculated = when; };
  void set_filename  (const char* fname)  { filename = fname;  };
  void set_subint    (int isbint)  { subint = isbint;   };
  void set_subband   (int isbband) { subband = isbband; };
  void set_subpoln   (int isbpoln) { subpoln = isbpoln; };

  Instrument get_instrument() { return instrument; };
  Format get_format    () { return format; };
  double get_frequency () { return frequency; };
  MJD    get_arrival   () { return arrival; };
  float  get_error     () { return error; };
  int    get_telescope () { return telescope; };
  time_t get_when_calculated () { return calculated; };
  string get_filename  () { return filename; };
  int    get_subint    () { return subint;   };
  int    get_subband   () { return subband; };
  int    get_subpoln   () { return subpoln; };

  bool is_deleted ()  const { return deleted == 1; };
  bool is_selected () const { return selected == 1; };
  void set_deleted (bool yup) { if (yup) deleted = 1; else deleted = 0; };
  void set_selected (bool yup) { if (yup) selected = 1; else selected = 0; };

  double shift (const polyco & poly) const;

  // loading and unloading to/from file and string
  int    load   (FILE* instream);
  int    load   (const char* instring);
  int    unload (FILE* outstream, Format fmt = Unspecified) const;
  int    unload (char* outstring, Format fmt = Unspecified) const;

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
		     Format fmt = Unspecified);
  static int unload (FILE* outstream, const vector<toa>& toas,
		     Format fmt = Unspecified);
  
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

  toa_model & operator=(const toa_model & toamodel);

  void load(const char * filename);
  void unload(const char * filename);
};

#endif
