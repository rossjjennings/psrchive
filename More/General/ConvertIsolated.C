/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ConvertIsolated.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

//! Default constructor
Pulsar::ConvertIsolated::ConvertIsolated ()
{
  neighbourhood = 0.01;
  test_value = 0.0;
  convert_value = 1.0;
}

//! Set the smoothing neighbourhood fraction
void Pulsar::ConvertIsolated::set_neighbourhood (float fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::ConvertIsolated::set_neighbourhood",
		 "invalid neighbourhood fraction = %lf", fraction);
  neighbourhood = fraction;
}

//! Get the smoothing neighbourhood fraction
float Pulsar::ConvertIsolated::get_neighbourhood () const
{
  return neighbourhood;
}

// #define _DEBUG 1

void Pulsar::ConvertIsolated::calculate (PhaseWeight* weight)
{
  unsigned nbin = input_weight->get_nbin();
  unsigned ncheck = nbin * neighbourhood;

  if (ncheck < 2)
    ncheck = 2;

#ifdef _DEBUG
  cerr << "Pulsar::ConvertIsolated::calculate ncheck=" << ncheck << endl;
#endif

  for (unsigned ibin=0; ibin < nbin; ibin++)
    (*weight)[ibin] = (*input_weight)[ibin];

  bool skipped = false;

  for (unsigned ibin=0; ibin < nbin; ibin++) {

#ifdef _DEBUG
    cerr << ibin << " " << (*input_weight)[ibin] << ": ";
#endif

    if ( (*weight)[ibin] != test_value ) {
#ifdef _DEBUG
      cerr << endl;
#endif
      skipped = false;
      continue;
    }

    // count the equal elements following this bin
    unsigned nequal = 0;
    for (unsigned jbin=0; jbin < ncheck; jbin++)
      if ((*input_weight)[ (ibin+jbin+nbin+1) % nbin ] == test_value)
	nequal ++;

#ifdef _DEBUG
    cerr << nequal << " ";
#endif

    if (nequal == ncheck) {
#ifdef _DEBUG
      cerr << "a" << endl;
#endif
      ibin += ncheck + 1;
      skipped = true;
      continue;
    }

    // if have previously skipped, then the following test is true by def'n
    if (skipped) {
      skipped = false;
      continue;
    }

    skipped = false;

    // count the equal elements preceding this bin
    nequal = 0;
    for (unsigned jbin=0; jbin < ncheck; jbin++)
      if ( (*input_weight)[ ((ibin+jbin+nbin) - ncheck) % nbin ] == test_value)
	nequal ++;

#ifdef _DEBUG
    cerr << nequal << " ";
#endif

    if (nequal == ncheck) {
#ifdef _DEBUG
      cerr << "b" << endl;
#endif
      continue;
    }

#ifdef _DEBUG
    cerr << "Pulsar::ConvertIsolated::calculate converting ibin=" 
	 << ibin << endl;
#endif

    (*weight)[ibin] = convert_value;

  }
}
