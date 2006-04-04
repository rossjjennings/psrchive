/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/InterpreterVariables.h"
#include "Functor.h"

#include "Pulsar/NoiseStatistics.h"
#include "Pulsar/Profile.h"

Pulsar::Interpreter::Variables::Variables ()
{
  add( Functor<double(const Archive*)> (this, &Variables::get_snr),
       "snr", "Total signal-to-noise ratio" );

  add( Functor<double(const Archive*)> (this, &Variables::get_nfnr),
       "nfnr", "Total noise-to-Fourier-noise ratio" );
}

//! Get the signal-to-noise ratio
double Pulsar::Interpreter::Variables::get_snr (const Archive* archive) const
{
  Reference::To<Archive> total = archive->total();
  return total->get_Profile(0,0,0)->snr();
}

//! Get the Fourier-noise-to-noise ratio
double Pulsar::Interpreter::Variables::get_nfnr (const Archive* archive) const
{
  Reference::To<Archive> total = archive->total();
  NoiseStatistics noise;
  return noise.get_nfnr(total->get_Profile(0,0,0));
}

