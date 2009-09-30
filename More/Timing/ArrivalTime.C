/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArrivalTime.h"
#include "Pulsar/ShiftEstimator.h"
#include "Pulsar/Archive.h"

using namespace std;

Pulsar::ArrivalTime::ArrivalTime ()
{
  format = Tempo::toa::Parkes;
  skip_bad = true;
}

Pulsar::ArrivalTime::~ArrivalTime ()
{
}

//! Set the observation from which the arrival times will be derived
void Pulsar::ArrivalTime::set_observation (const Archive* archive)
{
  observation = archive;
}

//! Set the standard/template used by some phase shift estimators
void Pulsar::ArrivalTime::set_standard (const Archive* archive)
{
  standard = archive;
}

//! Set the algorithm used to estimate the phase shift
void Pulsar::ArrivalTime::set_shift_estimator (ShiftEstimator* shift)
{
  shift_estimator = shift;
}

//! Set the format of the output time-of-arrival estimates
void Pulsar::ArrivalTime::set_format (Tempo::toa::Format fmt)
{
  format = fmt;
}

//! Add to the vector of time-of-arrival estimates
void  Pulsar::ArrivalTime::get_toas (std::vector<Tempo::toa>&)
{


}

void Pulsar::ArrivalTime::set_format (const string& fmt)
{
  if (strcasecmp(fmt.c_str(),"parkes")==0)
    format = Tempo::toa::Parkes;

  else if (strcasecmp(fmt.c_str(),"princeton")==0)
    format = Tempo::toa::Princeton;

  else if (strcasecmp(fmt.c_str(),"itoa")==0)
    format = Tempo::toa::ITOA;

  else if (strcasecmp(fmt.c_str(),"psrclock")==0)
    format = Tempo::toa::Psrclock;

  else if (strcasecmp(fmt.c_str(),"tempo2")==0)
    format = Tempo::toa::Tempo2;
}


