//-*-C++-*-
/* $Source: /cvsroot/psrchive/psrchive/Util/units/tostring.h,v $
   $Revision: 1.1 $
   $Date: 2004/11/22 11:18:33 $
   $Author: straten $ */

#ifndef __TOSTRING_H
#define __TOSTRING_H

#include <string>
#include <sstream>

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

