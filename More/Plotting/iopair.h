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
