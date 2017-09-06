//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
// psrchive/Util/units/substitute.h

#ifndef __UTILS_UNITS_SUBSTITUTE_H
#define __UTILS_UNITS_SUBSTITUTE_H

#include "TextInterfaceName.h"
#include "Functor.h"

#include <string>
#include <algorithm>
#include <ctype.h>

//! Return first character in text such that pred(c) is true
template<class Pred> std::string::size_type 
find_first_if (const std::string& text, Pred pred, std::string::size_type pos)
{
  std::string::const_iterator iter;
  iter = std::find_if (text.begin()+pos, text.end(), pred);
  if (iter == text.end())
    return std::string::npos;
  else
    return iter - text.begin();
}

template<class T>
std::string substitute (const std::string& text, const T* resolver,
			char substitution = '$',
			Functor< bool(char) > in_name = 
			Functor< bool(char) > (new TextInterface::Name,
					       &TextInterface::Name::valid) )
try
{
  std::string remain = text;
  std::string result;

  std::string::size_type start;
  
  while ( (start = remain.find(substitution)) != std::string::npos ) {

    // string preceding the variable substitution
    std::string before = remain.substr (0, start);

    // ignore the substitution symbol
    start ++;

    // repeated substitution symbol strings are simply decreased by one symbol
    std::string::size_type name_start = start;
    while (name_start < remain.length() && remain[name_start] == substitution)
      name_start ++;

    // find the end of the variable name
    std::string::size_type end;
    end = find_first_if (remain, std::not1(in_name), name_start);

    // length to end of variable name
    std::string::size_type length = std::string::npos;

    if (end != std::string::npos)
      length = end - start;

    // the variable name
    std::string name = remain.substr (start, length);

    if (start == name_start)
      // perform the substitution and add to the result
      result += before + resolver->get_value(name);
    else
      result += before + name;
    
    // remainder of string following the variable name
    if (end != std::string::npos)
      remain = remain.substr (end);
    else
      remain.erase();
  }

  return result + remain;
}
 catch (Error& error)
   {
     throw error += "substitute (text=\"" + text + "\",parser=" +
       resolver->get_interface_name() + ")";
   }

#endif
