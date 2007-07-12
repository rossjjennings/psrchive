/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/MaskSmooth.h"
#include "Pulsar/PhaseWeight.h"

using namespace std;

//! Default constructor
Pulsar::MaskSmooth::MaskSmooth ()
{
  window = 0.01;
  masked = 0.50;
}

//! Set the smoothing window fraction
void Pulsar::MaskSmooth::set_window (double fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::MaskSmooth::set_window",
		 "invalid window fraction = %lf", fraction);
  window = fraction;
}

//! Get the smoothing window fraction
double Pulsar::MaskSmooth::get_window () const
{
  return window;
}

//! Set the fraction of masked neighbours required
void Pulsar::MaskSmooth::set_masked (double fraction)
{
  if (fraction <= 0.0 || fraction >= 1.0)
    throw Error (InvalidParam, "Pulsar::MaskSmooth::set_masked",
		 "invalid masked fraction = %lf", fraction);
  masked = fraction;
}

//! Get the fraction of masked neighbours required
double Pulsar::MaskSmooth::get_masked () const
{
  return masked;
}

void Pulsar::MaskSmooth::calculate (PhaseWeight* weight)
{
  unsigned nbin = input_weight->get_nbin();
  unsigned iwindow = nbin * window * 0.5;
  unsigned imasked = iwindow * masked;

  cerr << "iwindow=" << iwindow << endl;

  for (unsigned ibin=0; ibin < nbin; ibin++) {

    cerr << ibin << " " << (*input_weight)[ibin] << ": ";

    if ((*input_weight)[ibin]) {
      (*weight)[ibin] = true;
      cerr << "t" << endl;
      continue;
    }

    // count the masked bins before this bin
    unsigned nmasked = 0;
    for (unsigned jbin=0; jbin < iwindow; jbin++)
      if ((*input_weight)[ (ibin + jbin - iwindow + nbin ) % nbin ])
	nmasked ++;

    cerr << nmasked << " ";

    if (nmasked < imasked) {
      cerr << endl;
      continue;
    }

    // count the masked bins after this bin
    nmasked = 0;
    for (unsigned jbin=0; jbin < iwindow; jbin++)
      if ((*input_weight)[ (ibin + jbin + 1 + nbin ) % nbin ])
	nmasked ++;

    cerr << nmasked << " ";

    if (nmasked < imasked) {
      cerr << endl;
      continue;
    }

    cerr << "Pulsar::MaskSmooth::calculate filling ibin=" << ibin << endl;
    (*weight)[ibin] = true;

  }
}
