//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/toa.h,v $
   $Revision: 1.3 $
   $Date: 2000/05/22 17:40:28 $
   $Author: straten $ */

#ifndef __TOA_H
#define __TOA_H

#include <vector>
#include <string>

#include "MJD.h"
#include "residual.h"

class polyco;   // tempo-generated polynomial describing pulsar phase = f(MJD)
class toaInfo;  // context specific pulsar information base class

namespace Tempo {

  class toa
  {

  public:
    
    enum Format { Unspecified, Princeton, Parkes, ITOA, Psrclock, Rhythm };
    enum State { Normal, Selected, Deleted };

    static bool verbose;
    static bool load_aux_data;

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
    string auxinfo;      /* text information passed to context specific data */

    toaInfo* auxdata;    /* context specific data */

    // one of the available formats on loading
    Format format;
    // state flag used when managing groups of toa objects
    State  state;

  public:
    // residual for this toa as calculated by tempo
    residual resid;
    
    toa () { init(); };
    ~toa () { destroy(); };
    
    // copy constructor
    toa (const toa & in_toa);
    
    // construct from an open file
    toa (FILE* instream);
    
    // construct from a string
    toa (char* indata);
    
    // methods for setting/getting things (may eventually check validity)
    void set_format    (Format fmt)  { format = fmt; };
    void set_frequency (double freq) { frequency = freq; };
    void set_arrival   (MJD arrived) { arrival = arrived; };
    void set_error     (float err)   { error = err; };
    void set_telescope (int telcode) { telescope = telcode; };
    void set_when_calculated (time_t when) { calculated = when; };
    void set_auxilliary_text (const string& text) { auxinfo = text; };

    Format get_format    () const { return format; };
    double get_frequency () const { return frequency; };
    MJD    get_arrival   () const { return arrival; };
    float  get_error     () const { return error; };
    int    get_telescope () const { return telescope; };
    time_t get_when_calculated () const { return calculated; };
    string get_auxilliary_text () const { return auxinfo; };

    bool is_deleted ()  const { return state == Deleted; };
    bool is_selected () const { return state == Selected; };

    void set_deleted (bool yup) { state = (yup) ? Deleted : Normal; };
    void set_selected (bool yup) { state = (yup) ? Selected : Normal; };
    
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
  
}

#endif
