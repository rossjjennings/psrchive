//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/tostring.h,v $
   $Revision: 1.17 $
   $Date: 2009/05/06 13:30:17 $
   $Author: straten $ */

#ifndef __TOSTRING_H
#define __TOSTRING_H

#include "Error.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <limits>

extern unsigned tostring_precision;
extern bool tostring_places;

template<class T>
std::string tostring (const T& input,
		      unsigned precision = std::numeric_limits<T>::digits10)
{
  extern std::ostringstream* tostring_ost;

  if (!tostring_ost)
    tostring_ost = new std::ostringstream;

  if( tostring_places )
    *tostring_ost << setiosflags( std::ios::fixed );
  else
    *tostring_ost << resetiosflags( std::ios::fixed );
  
  if (tostring_precision)
    tostring_ost->precision(tostring_precision);
  else
    tostring_ost->precision(precision);

  tostring_ost->str("");
  *tostring_ost << input;

  if (tostring_ost->fail())
  {
    Error error (InvalidState, "tostring");
    error << "failed to convert " << input << " to string:";
    throw error;
  }

  return tostring_ost->str();
}

template<class T>
T fromstring (const std::string& input)
{
  extern std::istringstream* fromstring_ist;

  if (!fromstring_ist)
    fromstring_ist = new std::istringstream;

  fromstring_ist->clear();
  fromstring_ist->str(input);

  T retval;
  *fromstring_ist >> retval;

  if (fromstring_ist->fail())
    throw Error (InvalidState, "fromstring", "failed to parse '"+ input +"'");

  return retval;
}

// string class specializations
template<>
inline std::string tostring (const std::string& input, unsigned)
{
  return input;
}

template<>
inline std::string fromstring<std::string> (const std::string& input)
{
  return input;
}

// char* specialization
inline std::string tostring (const char* input)
{
  return input;
}

/*
  If you've already written a function that converts string to Type,
  and this function throws an exception of type Error when the string
  cannot be parsed, then you can use this template to implement an
  extraction operator (operator >>).  See Util/genutil/Types.C for an
  example. 
*/
template<typename Type, typename String2Type>
std::istream& extraction (std::istream& is, Type& t, String2Type string2type)
{
  std::streampos pos = is.tellg();
  std::string ss;

  try {
    is >> ss;
    t = string2type (ss);
  }
  catch (Error& e) {
    is.setstate(std::istream::failbit);
    is.seekg(pos);
  }
  return is;
}

#endif
