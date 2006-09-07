/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Backend.h"

//! Default constructor
Pulsar::Backend::Backend (const char* name) : Extension (name) 
{
  hand = Signal::Right;
  argument = Signal::Conventional;
  downconversion_corrected = false;
}

//! Copy constructor
Pulsar::Backend::Backend (const Backend& backend) : Extension (backend) 
{
  operator=(backend);
}

//! Operator =
const Pulsar::Backend& Pulsar::Backend::operator= (const Backend& backend)
{
  hand = backend.hand;
  argument = backend.argument;
  downconversion_corrected = backend.downconversion_corrected;
  return *this;
}

//! Get the hand of the backend
Signal::Hand Pulsar::Backend::get_hand () const
{
  return hand;
}

//! Set the hand of the backend
void Pulsar::Backend::set_hand (Signal::Hand h)
{
  hand = h;
}

//! Get the argument of the backend
Signal::Argument Pulsar::Backend::get_argument () const
{
  return argument;
}

//! Set the argument of the backend
void Pulsar::Backend::set_argument (Signal::Argument a)
{
  argument = a;
}

//! Return true if backend has compensated for lower sideband downconversion
bool Pulsar::Backend::get_downconversion_corrected () const
{
  return downconversion_corrected;
}

//! Set true if backend has compensated for lower sideband downconversion
void Pulsar::Backend::set_downconversion_corrected (bool c)
{
  downconversion_corrected = c;
}
