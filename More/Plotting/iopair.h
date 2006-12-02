/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <utility>
#include <iostream>

template<class T, class U>
std::ostream& operator << (std::ostream& os, std::pair<T,U> vals)
{
  return os << "(" << vals.first << "," << vals.second << ")";
}

template<class T, class U>
std::istream& operator >> (std::istream& is, std::pair<T,U>& vals)
{
  char c;
  is >> c;

  if (c != '(') {
    is.setstate(std::istream::failbit);
    return is;
  }

  is >> vals.first >> c >> vals.second;

  if (c != ',') {
    is.setstate(std::istream::failbit);
    return is;
  }

  is >> c;

  if (c != ')')
    is.setstate(std::istream::failbit);

  return is;
}

// Stretch min and max to normalized range
template<typename T>
void stretch (const std::pair<T,T>& norm, T& min, T& max)
{
  T diff = max - min;
  max = min + norm.second * diff;
  min = min + norm.first * diff;
}

// Stretch range to normalized range
template<typename T>
void stretch (const std::pair<T,T>& norm, std::pair<T,T>& range)
{
  stretch (norm, range.first, range.second);
}

template<typename T, typename U>
bool compair1 (const std::pair<T,U>& a, const std::pair<T,U>& b)
{
  return a.first < b.first;
}

template<typename T, typename U>
bool compair2 (const std::pair<T,U>& a, const std::pair<T,U>& b)
{
  return a.second < b.second;
}

