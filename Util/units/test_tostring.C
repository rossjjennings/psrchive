/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "tostring.h"
#include <iostream>

using namespace std;

template<typename T>
int test (T exp, const string& txt)
{
  T val = fromstring<T>(txt);
  if (val != exp) {
    cerr << "test fromstring error val=" << val << " expected=" << exp << endl;
    return -1;
  }

  string str = tostring(exp);
  if (str != txt) {
    cerr << "test tostring error str=" << str << " expected=" << txt << endl;
    return -1;
  }

  return 0;
}

int main ()
{
  cerr << "numeric_limits<int>::digits10="
       << numeric_limits<int>::digits10 << endl;

  if (test ((int)12345678, "12345678"))
    return -1;

  cerr << "numeric_limits<float>::digits10="
       << numeric_limits<float>::digits10 << endl;

  if (test ((float)3.5678, "3.5678"))
    return -1;

  cerr << "numeric_limits<double>::digits10="
       << numeric_limits<double>::digits10 << endl;

  if (test ((double)1.234567890123, "1.234567890123"))
    return -1;

  cerr << "numeric_limits<string>::digits10="
       << numeric_limits<string>::digits10 << endl;

  if (test (string("string test"), "string test"))
    return -1;

  cerr << "All tests passed" << endl;
  return 0;
}
