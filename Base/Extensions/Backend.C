#include "Pulsar/Backend.h"

//! Default constructor
Pulsar::Backend::Backend (const char* name) : Extension (name) 
{
  hand = Signal::Right;
  argument = Signal::Conventional;
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
