//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/units/tostring.h,v $
   $Revision: 1.3 $
   $Date: 2004/12/15 21:11:18 $
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
T fromstring (const std::string& ss)
{
  std::stringstream iost;
  iost << ss;

  T retval;
  iost >> retval;

  return retval;
}

#endif

