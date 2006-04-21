//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
/* $Source: /cvsroot/psrchive/psrchive/Util/units/substitute.h,v $
   $Revision: 1.4 $
   $Date: 2006/04/21 04:21:03 $
   $Author: straten $ */

#ifndef __UTILS_UNITS_SUBSTITUTE_H
#define __UTILS_UNITS_SUBSTITUTE_H

#include <string>
#include <algorithm>
#include <ctype.h>

//! Return true if c may belong to a valid TextInterface variable name
inline bool isvar (char c) 
{
  static std::string valid = "_[,-]:";
  return isalnum(c) || valid.find(c) != std::string::npos;
}

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

namespace mystd {

//! Returns !Pred(T)
template<class Pred, class T>
class negate {
public:
  negate (Pred p) { pred = p; }
  bool operator () (const T& t) const { return !pred(t); }
protected:
  Pred pred;
};

}

//! Return first character in text such that pred(c) is false
template<class P> std::string::size_type 
find_first_not_if (const std::string& text, P pred, std::string::size_type pos)
{
  mystd::negate<P,std::string::value_type> not_pred (pred);
  return find_first_if (text, not_pred, pos);
}

template<class T>
std::string substitute (const std::string& text, const T* resolver,
			char substitution = '$', bool(*pred)(char) = isvar)
{
  std::string remain = text;
  std::string result;

  std::string::size_type start;

  while ( (start = remain.find(substitution)) != std::string::npos ) {

    // string preceding the variable substitution
    std::string before = remain.substr (0, start);

    // ignore the substitution symbol
    start ++;

    // find the end of the variable name
    std::string::size_type end = find_first_not_if (remain, pred, start);

    // length to end of variable name
    std::string::size_type length = std::string::npos;

    if (end != std::string::npos)
      length = end - start;

    // the variable name
    std::string name = remain.substr (start, length);

    // perform the substitution and add to the result
    result += before + resolver->get_value(name);

    // remainder of string following the variable name
    if (end != std::string::npos)
      remain = remain.substr (end);
    else
      remain.erase();

  }

  return result + remain;
}

#endif
