//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/residual.h,v $
   $Revision: 1.6 $
   $Date: 2000/05/22 17:40:26 $
   $Author: straten $ */

#ifndef __RESIDUAL_H
#define __RESIDUAL_H

#include "psr_cpp.h"  // C++ standard work-arounds
#include <vector>

namespace Tempo {

  // values output by TEMPO (resid2.tmp)
  class residual {

  public:
    static bool verbose;

    bool valid;         // is this a valid residual?

    double mjd;         // --TOA (MJD, referenced to solar system barycenter)
    double turns;       // --Postfit residual (pulse phase, from 0 to 1)
    double seconds;     // --Postfit residual (seconds)
    double binaryphase; // --Orbital phase (where applicable)
    double obsfreq;     // --Observing frequency (in barycenter frame)
    double weight;      // --Weight of point in the fit
    double error;       // --Timing uncertainty (according to input file)
    double preres;      // --Prefit residual (seconds)
    
    residual () { init(); };
    residual (int lun);
    ~residual () { };
    
    void init ();

    // Construct from a FORTRAN logical unit number 
    // (must be open and ready for reading)
    int  load (int lun);
    
    // place a vector load function in the residual namespace
    static int load (int r2flun, char* filename, vector<residual>* residuals);
  };

}

#endif
