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
    return false;

  // 

  this->parse_rows ();
  like->parse_rows ();

  if (this->rows.size() != like->rows.size())
    return false;
      
  for (unsigned irow=0; irow < rows.size(); irow++)
    if (! this->rows[irow].equals( like->rows[irow] ))
      return false;

  // cerr << "Pulsar::TextParameters::equals return true" << endl;
  return true;
}

void Pulsar::TextParameters::load (FILE* fptr)
{
  text = "";
  if (stringload (&text, fptr) < 0)
    throw Error (FailedSys, "Pulsar::TextParameters::load", "stringload");

  rows.resize (0);
}

//! Unload to an open stream
void Pulsar::TextParameters::unload (FILE* fptr) const
{
  if (fputs (text.c_str(), fptr) == EOF)
    throw Error (FailedSys, "Pulsar::TextParameters::unload", "fputs");
}

void Pulsar::TextParameters::parse_rows () const
{
  if (rows.size() > 0)
    return;
  
  vector<string> lines;

  string_split_on_any ( text, lines, "\n" );

  // cerr << "TextParameters::parse_rows nlines=" << lines.size() << endl;

  string whitespace = " \t\n";
  
  for (unsigned iline=0; iline < lines.size(); iline++)
  {
    Row row;
    row.keyword = stringtok (lines[iline], whitespace);
    row.value = stringtok (lines[iline], whitespace);
    row.flag = stringtok (lines[iline], whitespace);

    if (row.keyword != "" && row.value != "")
      rows.push_back( row );
  }
}


//! Retrieve a string from the text
string Pulsar::TextParameters::get_value (const string& keyword) const
{
  const string whitespace = " \t\n";

  string empty;

  // find the start of the keyword
  string::size_type start = 0;

  while ( (start = text.find (keyword, start)) != string::npos )
  {
    // find the end of the keyword
    string::size_type end = text.find_first_of (whitespace, start);

    // keyword not followed by whitespace
    if (end == string::npos)
      return empty;

    // check that the keyword is preceded by whitespace
    if (! (start == 0 || whitespace.find( text[start-1] )))
    {
      start = end;
      continue;
    }

    // check that the keyword is followed by whitespace
    if (text.substr (start, end - start) != keyword)
    {
      start = end;
      continue;
    }

    // find the start of the value
    start = text.find_first_not_of (whitespace, end);

    // and the end of the value
    end = text.find_first_of (whitespace, start);

    return text.substr (start, end-start);
  }

  return empty;
}

string Pulsar::TextParameters::get_name () const
{
  string name = get_value ("PSR");
  if (name.empty())
    name = get_value ("PSRJ");
  if (name.empty())
    name = get_value ("PSRB");

  return name;
}

