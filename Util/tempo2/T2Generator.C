/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Generator.h"
#include "T2Predictor.h"
#include "T2Parameters.h"

#include "Error.h"
#include "file_cast.h"
#include "tempo2pred_int.h"

using namespace std;

Tempo2::Generator::Generator (const Parameters* parameters)
{
  epoch1 = epoch2 = 0;
  freq1 = freq2 = 0;

  ntimecoeff = 12;
  nfreqcoeff = 2;

  // one hour
  segment_length = 1.0/24.0;

  if (parameters)
    set_parameters( parameters );
}

Tempo2::Generator* Tempo2::Generator::clone () const
{
  return new Generator (*this);
}

Tempo2::Generator::~Generator ()
{
}

//! Set the parameters used to generate the predictor
void Tempo2::Generator::set_parameters (const Pulsar::Parameters* p) try
{
  parameters = dynamic_file_cast<const Tempo2::Parameters> (p);
}
catch (Error& error)
{
  throw error += "Tempo2::Generator::set_parameters (Pulsar::Parameters*)";
}

//! Set the range of epochs over which to generate
void Tempo2::Generator::set_time_span (const MJD& start, const MJD& finish)
{
  epoch1 = from_MJD (start);
  epoch2 = from_MJD (finish);
}

//! Set the range of frequencies over which to generate
void Tempo2::Generator::set_frequency_span (long double low, long double high)
{
  freq1 = low;
  freq2 = high;
}

//! Set the site at which the signal is observed
void Tempo2::Generator::set_site (const std::string& site)
{
  sitename = site;
}

//! Set the number of time coefficients
void Tempo2::Generator::set_time_ncoeff (unsigned ncoeff)
{
  ntimecoeff = ncoeff;
}

//! Set the number of frequency coefficients
void Tempo2::Generator::set_frequency_ncoeff (unsigned ncoeff)
{
  nfreqcoeff = ncoeff;
}

//! Set the length of each polynomial segment in days
void Tempo2::Generator::set_segment_length (long double days)
{
  segment_length = days;
}

//! Return a new, copy constructed instance of self
Pulsar::Predictor* Tempo2::Generator::generate () const
{
  if (Predictor::verbose)
    debugFlag = 1;

  Tempo2::Predictor* pred = new Tempo2::Predictor;

  if (!parameters->psr)
    throw Error (InvalidState, "Tempo2::Generator::generate",
		 "Tempo2::Parameters not properly initialized");

  const pulsar* psr = parameters->psr;

  ChebyModelSet* cms = &pred->predictor.modelset.cheby;
  pred->predictor.kind = Cheby;

  long double use_epoch1 = epoch1;
  long double use_epoch2 = epoch2;

  if (epoch1 == epoch2) {
    use_epoch1 -= 0.4 * segment_length;
    use_epoch2 += 0.4 * segment_length;
  }

  if (Predictor::verbose)
    cerr << "Tempo2::Generator::generate call ChebyModelSet_Construct\n" 
      " sitename=" << sitename <<
      " epoch1=" << use_epoch1 << " epoch2=" << use_epoch2 << "\n"
      " segment_length=" << segment_length <<
      " freq1=" << freq1 << " freq2=" << freq2 << "\n"
      " coeffs: ntime=" << ntimecoeff << " nfreq=" << nfreqcoeff
	 << endl;

  ChebyModelSet_Construct( cms, psr, sitename.c_str(), use_epoch1, use_epoch2,
			   segment_length, segment_length*0.1, 
			   freq1, freq2, ntimecoeff, nfreqcoeff );

  if (Predictor::verbose)
    cerr << "Tempo2::Generator::generate ChebyModelSet_Construct nsegment=" 
         << cms->nsegments << endl;

  long double rms, mav;
  ChebyModelSet_Test( cms, psr, ntimecoeff*5*cms->nsegments, 
		      nfreqcoeff*5*cms->nsegments, &rms, &mav );
  printf("RMS error = %.3Lg s MAV= %.3Lg s\n", 
	 rms/psr[0].param[param_f].val[0], mav/psr[0].param[param_f].val[0]);

  pred->set_observing_frequency (0.5L * (freq1 + freq2));

  return pred;
}


