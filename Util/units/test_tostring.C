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
  if (test (3, "3"))
    return -1;

  if (test (3.56, "3.56"))
    return -1;

  if (test (string("string test"), "string test"))
    return -1;

  cerr << "All tests passed" << endl;
  return 0;
}
