//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/tempo++.h,v $
   $Revision: 1.14 $
   $Date: 2003/06/30 04:53:39 $
   $Author: ahotan $ */

#ifndef __TEMPO_PP_H
#define __TEMPO_PP_H

#include <vector>

#include "MJD.h"
#include "poly.h"
#include "psrephem.h"
#include "toa.h"

namespace Tempo {
  
  // set the system call used to run tempo
  void   set_system (const char* system_call);
  // get the system call used to run tempo
  string get_system ();

  // get the tempo version
  float  get_version ();

  // set the directory in which tempo system calls will be made
  void   set_directory (const char* directory);
  // get the directory in which tempo system calls will be made
  string get_directory ();

  // convenience overloads
  void   set_system (const string& system_call);
  void   set_directory (const string& directory);

  // verbosity flag of functions working in the Tempo namespace
  extern bool verbose;
  extern bool debug;

  // extension added to temporary Model filenames when working
  extern string extension;

  // default value passed to predict functions
  extern MJD unspecified;

  // base directory in which tempo will work
  extern string tmpdir;

  // file to which tempo stderr output will be redirected
  extern string stderr_filename;

  // file to which tempo ephemeris files are written
  extern string ephem_filename;

  //! Run tempo using the given arguments and input
  /*! tempo is run with a working directory given by
    Tempo::get_directory, so input files should be written here and
    output files (created by tempo) can be found here. */
  void tempo (const string& arguments, const string& input);

  // given a tempo ephemeris, generate toas over the range of MJD given
  // with characteristics specififed by rms and error.
  //   model      - tempo ephemeris
  //   toas       - return value
  //   start,end  - delimit epoch
  //   interval   - separation between points in minutes
  //   rms        - gaussian noise specified in microseconds
  //   error      - width of normal distribution of errors (not implemented)
  void fake (vector<toa>& toas, const psrephem& model,
	     const MJD& start, const MJD& end, double interval,
	     float rms = 0.0, float error = 0.0, float lst_range = 8.0);

  // given pulsar parameters and times of arrival, calls TEMPO to determine
  // the residual TOA.
  void fit (const psrephem& model, vector<toa>& toas,
	    psrephem* postfit = NULL, bool track=false,
	    Tempo::toa::State min_state = Tempo::toa::Normal);
  
  // returns a polyco valid over the range in MJD specified by m1 and m2
  polyco get_polyco (const psrephem& eph,
		     const MJD& m1=unspecified, const MJD& m2=unspecified, 
		     double nspan=960, int ncoeff=12, int maxha=8, 
		     char tel='7', double centrefreq=1400.0);
  
  void set_polyco (polyco& poly, const psrephem& eph,
		   const MJD& m1=unspecified, const MJD& m2=unspecified, 
		   double nspan=960, int ncoeff=12, int maxha=8, 
		   char tel='7', double centrefreq=1400.0);
  
  polyco span (const polyco& first_poly, const polyco& second_poly,
	       const psrephem& pephem);
  
  // //////////////////////////////////////////////////////////////////////
  // Calculates the Lomb-Scargle periodogram from a set of residuals.
  // The code is written as a template in order that an array of any class
  // type that may be cast to a residual may be passed.
  template <class resType>
    void periodogram (vector<float>& frequency, vector<float>& power,
		      const vector<resType>& data,
		      float oversampling = 1.0, float overnyquist = 1.0);

#include "periodogram_impl.h"

}

#endif

