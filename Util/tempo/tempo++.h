//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/tempo++.h,v $
   $Revision: 1.6 $
   $Date: 2001/01/08 11:25:59 $
   $Author: straten $ */

#ifndef __TEMPOPLUSPLUS_H
#define __TEMPOPLUSPLUS_H

#include <vector>

#include "MJD.h"
#include "poly.h"
#include "toa.h"

class psrParams;
class psrephem;

namespace Tempo {
  
  // set the system call used to run tempo
  void   set_system (const char* system_call);
  void   set_system (const string& system_call);
  // get the system call used to run tempo
  string get_system ();

  // get the tempo version
  int    get_version ();

  // set the directory in which tempo system calls will be made
  void   set_directory (const char* directory);
  void   set_directory (const string& directory);
  // get the directory in which tempo system calls will be made
  string get_directory ();

  // verbosity flag of functions working in the Tempo namespace
  extern bool verbose;

  // extension added to temporary Model filenames when working
  extern string extension;

  // default value passed to predict functions
  extern MJD unspecified;

  // exception thrown on error
  class Error {
  protected:
    string msg;
  public:
    Error () { msg = "Tempo::Error"; }
    Error (const char* str) { msg = str; }
    Error (const string& str) { msg = str; }
    
    friend ostream& operator<< (ostream& ostr, const Error& error)
      { return ostr << error.msg; }
  };

  // given pulsar parameters and times of arrival, calls TEMPO to determine
  // the residual TOA.
  void fit (const psrParams& model, vector<toa>& toas,
	    psrParams* postfit = NULL, bool track=false,
	    DataPoint::State min_state = DataPoint::Normal);
  
  // returns a polyco valid over the range in MJD specified by m1 and m2
  polyco predict (const psrephem& eph,
		  const MJD& m1=unspecified, const MJD& m2=unspecified, 
		  double nspan=960, int ncoeff=12, int maxha=8, 
		  int tel=7, double centrefreq=1400.0);
  
  void predict (polyco* poly, const psrephem& eph,
		const MJD& m1=unspecified, const MJD& m2=unspecified, 
		double nspan=960, int ncoeff=12, int maxha=8, 
		int tel=7, double centrefreq=1400.0);
  
  polyco poly_span (const polyco & first_poly, 
		    const polyco & second_poly, const psrephem & pephem);
  
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
