/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Predictor.h"
#include "Error.h"

Tempo2::Predictor::Predictor ()
{
  T2Predictor_Init (&predictor);
}

Tempo2::Predictor::Predictor (const Predictor& copy)
{
  T2Predictor_Init (&predictor);
  T2Predictor_Copy (&predictor, &copy.predictor);
}

Tempo2::Predictor::~Predictor ()
{
  T2Predictor_Destroy (&predictor);
}

//! Return a new, copy constructed instance of self
Pulsar::Predictor* Tempo2::Predictor::clone () const
{
  return new Predictor (*this);
}

//! Return true if the supplied predictor is equal to self
bool Tempo2::Predictor::equals (const Pulsar::Predictor*) const
{
  throw Error (InvalidState, "Tempo2::Predictor::equals", "not implemented");
}

//! Add the information from the supplied predictor to self
void Tempo2::Predictor::insert (const Pulsar::Predictor* from)
{
  const Predictor* t2p = dynamic_cast<const Predictor*>(from);
  if (!t2p)
    throw Error (InvalidParam, "Tempo2::Predictor::insert",
		 "Predictor is not a Tempo2 Predictor");

  T2Predictor_Insert (&predictor, &t2p->predictor);
}

//! Set the observing frequency at which predictions will be made
void Tempo2::Predictor::set_observing_frequency (long double MHz)
{
  observing_frequency = MHz;
}

//! Get the observing frequency at which phase and epoch are related
long double Tempo2::Predictor::get_observing_frequency () const
{
  return observing_frequency;
}

//! convert an MJD to long double
long double from_MJD (const MJD& t)
{
  const long double secs_in_day = 86400.0L;

  return 
    (long double) (t.intday()) +
    (long double) (t.get_secs()) / secs_in_day +
    (long double) (t.get_fracsec()) / secs_in_day;
}

//! convert a long double to Phase
Phase to_Phase (long double p)
{
  int64 turns = int64 (p);
  double fturns = p - turns;

  return Phase (turns, fturns);
}

//! Return the phase, given the epoch
Phase Tempo2::Predictor::phase (const MJD& t) const
{
  return to_Phase( T2Predictor_GetPhase (&predictor, from_MJD (t),
					 observing_frequency) );
}

//! Return the epoch, given the phase
MJD Tempo2::Predictor::iphase (const Phase& phase) const
{
  throw Error (InvalidState, "Tempo2::Predictor::iphase", "not implemented");
}

//! Return the spin frequency, given the epoch
long double Tempo2::Predictor::frequency (const MJD& t) const
{
  return T2Predictor_GetFrequency (&predictor, from_MJD (t),
				   observing_frequency);
}

//! Return the phase correction for dispersion delay
Phase Tempo2::Predictor::dispersion (const MJD &t, long double MHz) const
{
  throw Error (InvalidState, "Tempo2::Predictor::dispersion",
	       "not implemented"); 
}

void Tempo2::Predictor::load (FILE* fptr)
{
  T2Predictor_FRead (&predictor, fptr);

  observing_frequency = 0.5L *
    (T2Predictor_GetStartFreq(&predictor)+T2Predictor_GetEndFreq(&predictor));
}

void Tempo2::Predictor::unload (FILE* fptr) const
{
  T2Predictor_FWrite (&predictor, fptr);
}
