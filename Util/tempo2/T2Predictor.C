/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Predictor.h"

//! Return a new, copy constructed instance of self
Pulsar::Predictor* Tempo2::Predictor::clone () const
{
}

//! Return true if the supplied predictor is equal to self
bool Tempo2::Predictor::equals (const Pulsar::Predictor*) const
{
}

//! Set the observing frequency at which predictions will be made
void Tempo2::Predictor::set_observing_frequency (long double MHz)
{
}

//! Get the observing frequency at which phase and epoch are related
long double Tempo2::Predictor::get_observing_frequency () const
{
}

//! Return the phase, given the epoch
Phase Tempo2::Predictor::phase (const MJD& t) const
{
}

//! Return the epoch, given the phase
MJD Tempo2::Predictor::iphase (const Phase& phase) const
{
}

//! Return the spin frequency, given the epoch
long double Tempo2::Predictor::frequency (const MJD& t) const
{
}

//! Return the phase correction for dispersion delay
Phase Tempo2::Predictor::dispersion (const MJD &t, long double MHz) const
{
}


