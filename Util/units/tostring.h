//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/units/tostring.h,v $
   $Revision: 1.4 $
   $Date: 2004/12/16 16:29:07 $
   $Author: straten $ */

#ifndef __TOSTRING_H
#define __TOSTRING_H

#include <string>
#include <sstream>
#include <iomanip>

template<class T>
std::string tostring (const T& input)
{
  std::ostringstream ost;
  ost << input;
  return ost.str();
}

template<class T>
std::string tostring (const T& input, unsigned precision)
{
  std::ostringstream ost;
  ost << std::setprecision( precision ) << input;
  return ost.str();
}

template<class T>
T fromstring (const std::string& input)
{
  std::stringstream iost;
  iost << input;

  T retval;
  iost >> retval;

  return retval;
}

// string class specializations
std::string tostring (const std::string& input)
{
  return input;
}

template<>
std::string fromstring<std::string> (const std::string& input)
{
  return input;
}


#endif


