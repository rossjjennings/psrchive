//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/units/tostring.h,v $
   $Revision: 1.2 $
   $Date: 2004/11/23 10:54:27 $
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


#endif

