/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Generator.h"
#include "T2Predictor.h"
#include "Error.h"

#include "tempo2pred_int.h"

Tempo2::Generator::Generator ()
{
}

Tempo2::Generator::Generator (const Generator& copy)
{
}

Tempo2::Generator::~Generator ()
{
}

//! Return a new, copy constructed instance of self
Pulsar::Predictor* Tempo2::Generator::generate () const
{
  // how to initialize?
  pulsar* psr = 0;

  // length of each segment in days
  long double seg_length;
  int ntimecoeff, nfreqcoeff;
  char sitename[64];
  long double mjd_start, mjd_end;
  long double freq_start, freq_end;

  ChebyModelSet cms;
  ChebyModelSet_Construct(&cms, psr, sitename, mjd_start, mjd_end,
			  seg_length, seg_length*0.1, 
			  freq_start, freq_end, ntimecoeff, nfreqcoeff);

  long double rms, mav;
  ChebyModelSet_Test(&cms, psr, ntimecoeff*5*cms.nsegments, 
		     nfreqcoeff*5*cms.nsegments, &rms, &mav);
  printf("Predictive model constructed and written to t2pred.dat.\n");
  printf("RMS error = %.3Lg s MAV= %.3Lg s\n", 
	 rms/psr[0].param[param_f].val[0], mav/psr[0].param[param_f].val[0]);

  Tempo2::Predictor* pred = new Tempo2::Predictor;
  pred->predictor.modelset.cheby = cms;
}


