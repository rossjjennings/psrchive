//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/toa.h,v $
   $Revision: 1.11 $
   $Date: 2003/04/04 07:46:00 $
   $Author: ahotan $ */

#ifndef __TOA_H
#define __TOA_H

#include <vector>
#include <string>

#include "residual.h"
#include "DataPoint.h"
#include "MJD.h"

class polyco;   // tempo-generated polynomial describing pulsar phase = f(MJD)

namespace Tempo {

  class toaInfo;

  // //////////////////////////////////////////////////////////////////////////
  //
  // toa - class that encapsulates TEMPO data format
  //
  // //////////////////////////////////////////////////////////////////////////

  class toa : public DataPoint
  {

  public:
    
    enum Format { Unspecified, Princeton, Parkes, ITOA, Psrclock, Rhythm };
    enum DataType {
      // a null state
      Nothing,
      // from the toa class
      Frequency,
      Arrival,
      Sigma,
      Telescope,
      PhaseOffset,
      DMCorrection,
      // from the residual
      BarycentreArrival,
      ResidualPhase,
      ResidualTime,
      BinaryPhase,
      BarycentreFrequency,
      Weight,
      PrefitResidualTime,
      // and a code to mark the last
      Last
    };

    static bool verbose;

  protected:
    
    // fundamental TOA LINE as on:
    // http://pulsar.princeton.edu/tempo/ref_man_sections/toa.txt
    double frequency;      // Observing frequency (MHz)
    MJD    arrival;        // TOA
    float  error;          // TOA uncertainty
    char    telescope;      // Observatory (one-character code)
    
    // Parkes Format specific
    float  phs;            // Phase offset (fraction of P0, added to TOA)
    // Princeton and ITOA Format specfic
    float  dmc;            // DM correction (pc cm^-3)
    // ITOA Format specific
    char   observatory[2]; // Observatory (two-letter code)
    
    // Rhythm extras
    time_t calculated;   /* the date when this toa was calculated */
    string auxinfo;      /* text information passed to context specific data */

    // Reference::To<toaInfo> auxdata;    /* context specific data */

    // one of the available formats on loading
    Format format;

  public:
    // residual for this toa as calculated by tempo
    residual resid;
   
    toa (Format fmt = Rhythm);
    virtual ~toa () { destroy(); };
    
    // copy constructor
    toa (const toa & in_toa);
    toa& operator = (const toa & in_toa);

    // construct from an open file
    toa (FILE* instream);
    
    // construct from a string
    toa (char* indata);
    
    // methods for setting/getting things (may eventually check validity)
    void set_format    (Format fmt)  { format = fmt; };
    void set_frequency (double freq) { frequency = freq; };
    void set_arrival   (MJD arrived) { arrival = arrived; };
    void set_error     (float err)   { error = err; };
    void set_telescope (char telcode) { telescope = telcode; };
    void set_when_calculated (time_t when) { calculated = when; };
    void set_auxilliary_text (const string& text) { auxinfo = text; };

    Format get_format    () const { return format; };
    double get_frequency () const { return frequency; };
    MJD    get_arrival   () const { return arrival; };
    float  get_error     () const { return error; };
    char   get_telescope () const { return telescope; };
    time_t get_when_calculated () const { return calculated; };
    string get_auxilliary_text () const { return auxinfo; };

    // //////////////////////////////////////////////////////////////////
    // these functions return information in a context-free fashion that
    // allows inherited types to add to the class without changing
    // high-level routines (such as ModelDataSet
    //
    // abstract interface for query
    virtual double      getData (DataType which) const;
    virtual const char* getDescriptor (DataType code) const;

    double shift (const polyco & poly) const;

    void get_az_zen_para (double ra, double dec,
			  float& az, float& zen, float& para) const;

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
    string Psrclock_unload  () const;

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
    

  // //////////////////////////////////////////////////////////////////////////
  //
  // toaInfo - abstract base class for context-specific information
  //
  // inherit this class when you wish to add additional information to the
  // toa class in a generic way.
  //
  // //////////////////////////////////////////////////////////////////////////

  class toaInfo {
// : public Reference::Able {

  public:
    virtual ~toaInfo ();
    // the value of this object at serial number
    virtual double getData (toa::DataType code) const = 0;
    // the short descriptive string corresponding to the serial number
    virtual const char* getDescriptor (toa::DataType code) const = 0;

  };

}

#endif
