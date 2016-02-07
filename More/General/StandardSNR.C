/***************************************************************************
 *
 *   Copyright (C) 2004-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardSNR.h"

void Pulsar::StandardSNR::set_standard (const Profile* profile)
{
  fit.choose_maximum_harmonic = true;
  fit.set_standard (profile);
}

float Pulsar::StandardSNR::get_snr (const Profile* profile)
{
  fit.set_Profile (profile);
  return fit.get_snr();
}    

class Pulsar::StandardSNR::Interface
  : public TextInterface::To<StandardSNR>
{
public:
  Interface (StandardSNR* instance)
  {
    if (instance)
      set_instance (instance);
  }

  std::string get_interface_name () const { return "standard"; }
};


//! Return a text interface that can be used to configure this instance
TextInterface::Parser* Pulsar::StandardSNR::get_interface ()
{
  return new Interface (this);
}

//! Return a copy constructed instance of self
Pulsar::StandardSNR* Pulsar::StandardSNR::clone () const
{
  return new StandardSNR (*this);
}
