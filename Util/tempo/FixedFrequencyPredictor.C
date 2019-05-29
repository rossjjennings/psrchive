/***************************************************************************
 *
 *   Copyright (C) 2015 by Matthew Kerr
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FixedFrequencyPredictor.h"

//! convert a long double to Phase
static Pulsar::Phase to_Phase (long double p)
{
  int64_t turns = int64_t (p);
  double fturns = p - turns;

  return Pulsar::Phase (turns, fturns);
}

Pulsar::FixedFrequencyPredictor::FixedFrequencyPredictor () {
  m_freq = 0;
}

Pulsar::FixedFrequencyPredictor::FixedFrequencyPredictor (long double Hz) {
  m_freq = Hz;
}

void Pulsar::FixedFrequencyPredictor::set_topocentric_frequency (long double Hz) {
  m_freq = Hz;
}

long double Pulsar::FixedFrequencyPredictor::get_topocentric_frequency () const {
  return m_freq;
}

void Pulsar::FixedFrequencyPredictor::set_observing_frequency (long double MHz) {
  m_obs_freq = MHz;
}

long double Pulsar::FixedFrequencyPredictor::get_observing_frequency () const {
  return m_obs_freq;
}

void Pulsar::FixedFrequencyPredictor::set_epoch (const MJD& t) {
  m_epoch = t;
}

const MJD & Pulsar::FixedFrequencyPredictor::get_epoch() const {
  return m_epoch;
}

Pulsar::Phase Pulsar::FixedFrequencyPredictor::phase (const MJD& t) const {
  MJD dt = t-m_epoch;
  long double prod = dt.in_seconds()*m_freq;
  return to_Phase(prod);
}

MJD Pulsar::FixedFrequencyPredictor::iphase (const Pulsar::Phase& phase, const MJD*) const {
  MJD days = phase / m_freq;
  return days + m_epoch;
}

long double Pulsar::FixedFrequencyPredictor::frequency (const MJD&) const {
  return m_freq;
}

//! Return the phase correction for dispersion delay
Pulsar::Phase Pulsar::FixedFrequencyPredictor::dispersion (const MJD&, long double) const {
  return to_Phase(0.);
}
