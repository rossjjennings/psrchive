/***************************************************************************
 *
 *   Copyright (C) 2004-2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/StandardSNR.h"
#include "Pulsar/Archive.h"

void Pulsar::StandardSNR::set_standard (const Profile* profile)
{
  fit.choose_maximum_harmonic = true;
  fit.set_standard (profile);
}

float Pulsar::StandardSNR::get_snr (const Profile* profile) try
{
  fit.set_Profile (profile);
  return fit.get_snr();
}    
 catch (Error& error)
   {
     throw error += "Pulsar::StandardSNR::get_snr";
   }

//! Set the name of the file from which the standard profile will be loaded
void Pulsar::StandardSNR::set_standard_filename (const std::string& fname)
{
  Reference::To<Pulsar::Archive> archive = Pulsar::Archive::load (fname);
  set_standard (archive->get_Profile(0,0,0));
  filename = fname;
}

//! Return the name of the file from which the standard was loaded
std::string Pulsar::StandardSNR::get_standard_filename () const
{
  return filename;
}

class Pulsar::StandardSNR::Interface
  : public TextInterface::To<StandardSNR>
{
public:
  Interface (StandardSNR* instance)
  {
    if (instance)
      set_instance (instance);

    add( &StandardSNR::get_standard_filename,
         &StandardSNR::set_standard_filename,
         "file", "filename of standard" );
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
