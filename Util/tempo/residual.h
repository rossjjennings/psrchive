#ifndef __RESIDUAL_H
#define __RESIDUAL_H

#include "psr_cpp.h"  // C++ standard work-arounds
#include <vector>

// values output by TEMPO (resid2.tmp)
class residual {
 public:
  static int verbose;

  enum   plot { BinaryPhase, Mjd, Seconds, Turns };
  static plot xtype;
  static plot ytype;

  double mjd;         // --TOA (MJD, referenced to solar system barycenter)
  double turns;       // --Postfit residual (pulse phase, from 0 to 1)
  double seconds;     // --Postfit residual (seconds)
  double binaryphase; // --Orbital phase (where applicable)
  double obsfreq;     // --Observing frequency (in barycenter frame)
  double weight;      // --Weight of point in the fit
  double error;       // --Timing uncertainty (according to input file)
  double preres;      // --Prefit residual (seconds)

  residual () { init(); };
  residual (int lun) { if (load(lun)<0) throw ("residual construct"); };
  ~residual () { };

  void init ();
  // Construct from a FORTRAN logical unit number (open and ready for reading)
  int  load (int lun);

  // place a load function in the residual namespace
  static int load (int r2flun, char* filename, vector<residual>* residuals);
};

float y_ordinate (const residual& r);
float x_ordinate (const residual& r);
float y_error (const residual& r);
float x_error (const residual& r);

#endif
