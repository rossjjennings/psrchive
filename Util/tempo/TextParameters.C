/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/TextParameters.h"
#include "strutil.h"

using namespace std;

Pulsar::Parameters* Pulsar::TextParameters::clone () const
{
  return new TextParameters (*this);
}

bool Pulsar::TextParameters::equals (const Parameters* that) const
{
  const TextParameters* like = dynamic_cast<const TextParameters*>( that );

  if (!like)
    throw Error (InvalidState, "Pular::TextParameters::equals",
		 "Parameters is not a TextParamters");

  return this->text == like->text;
}

void Pulsar::TextParameters::load (FILE* fptr)
{
  text = "";
  if (stringload (&text, fptr) < 0)
    throw Error (FailedSys, "Pulsar::TextParameters::load", "stringload");
}

//! Unload to an open stream
void Pulsar::TextParameters::unload (FILE* fptr) const
{
  if (fputs (text.c_str(), fptr) == EOF)
    throw Error (FailedSys, "Pulsar::TextParameters::unload", "fputs");
}

//! Retrieve a string from the text
string Pulsar::TextParameters::get_value (const string& keyword) const
{
  const string whitespace = " \t\n";

  string result;

  // find the start of the keyword
  string::size_type start = text.find (keyword);

  // no keyword
  if (start == string::npos)
    return result;

  // find the end of the keyword
  string::size_type end = text.find_first_of (whitespace, start);

  // keyword not followed by whitespace
  if (end == string::npos)
    return result;

  // find the start of the value
  start = text.find_first_not_of (whitespace, end);

  // and the end of the value
  end = text.find_first_of (whitespace, start);

  return text.substr (start, end);
}

string Pulsar::TextParameters::get_name () const
{
  return get_value ("PSR");
}
