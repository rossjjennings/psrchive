//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/tempo++.h,v $
   $Revision: 1.4 $
   $Date: 2000/05/22 17:40:26 $
   $Author: straten $ */

#ifndef __TEMPOPLUSPLUS_H
#define __TEMPOPLUSPLUS_H

#include <vector>

#include "MJD.h"
#include "poly.h"

class psrParams;
class psrephem;

namespace Tempo {

  class toa;
  class residual;

  extern bool verbose;
  extern MJD  unspecified;

  // exception thrown on error
  class Error {
  protected:
    string msg;
  public:
    Error (const char*);
    Error (const string&);

    friend ostream& operator<< (ostream& ostr, const Error& error);
  };

  // given pulsar parameters and times of arrival, calls TEMPO to determine
  // the residual TOA.
  void fit (const psrParams& model, vector<toa>& toas,
	    psrParams* postfit = NULL, bool track = false);

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

}

#endif
