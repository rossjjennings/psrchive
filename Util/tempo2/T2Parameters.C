/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "T2Parameters.h"
#include "T2Generator.h"

using namespace std;

//! Default constructor
Tempo2::Parameters::Parameters ()
{
  if (verbose)
    cerr << "Tempo2::Parameters constructor" << endl;

  int noWarnings = 1;
  int fullSetup = 0;

  initialiseOne (&psr, noWarnings, fullSetup);

  if (verbose)
    cerr << "Tempo2::Parameters initialiseOne completed" << endl;

  if (setupParameterFileDefaults (&psr) < 0)
    throw Error (InvalidState, "Tempo2::Parameters ctor", TEMPO2_ERROR);

  if (verbose)
    cerr << "Tempo2::Parameters setupParameterFileDefaults completed" << endl;
}

//! Destructor
Tempo2::Parameters::~Parameters ()
{
}

//! Copy constructor
Tempo2::Parameters::Parameters (const Parameters&)
{
  throw Error (InvalidState, "Tempo2::Parameters",
	       "copy constructor not implemented");
}

Pulsar::Parameters* Tempo2::Parameters::clone () const
{
  return new Parameters (*this);
}

Pulsar::Generator* Tempo2::Parameters::generator () const
{
  return new Generator (this);
}

//! Return true if *this == *that
bool Tempo2::Parameters::equals (const Pulsar::Parameters* that)
{
  const Parameters* like = dynamic_cast<const Parameters*>( that );
  throw Error (InvalidState, "Tempo2::Parameters::equals", "not implemented");
}

//! Load from an open stream
void Tempo2::Parameters::load (FILE* fptr)
{
  cerr << "Tempo2::Parameters::load (FILE*)" << endl;
  readSimpleParfile (fptr, &psr);
  cerr << "Tempo2::Parameters::load readSimpleParfile completed" << endl;
}

//! Unload to an open stream
void Tempo2::Parameters::unload (FILE*) const
{
  throw Error (InvalidState, "Tempo2::Parameters::unload", "not implemented");
}

//! Return the name of the source
std::string Tempo2::Parameters::get_name () const
{
  return "Tempo2::Paramters::get_name unknown";
}

//! Return the coordinates of the source
sky_coord Tempo2::Parameters::get_coordinates () const
{
  return sky_coord();
}

//! Return the dispersion measure
double Tempo2::Parameters::get_dispersion_measure () const
{
  return 0;
}
  
//! Return the rotation measure
double Tempo2::Parameters::get_rotation_measure () const
{
  return 0;
}
