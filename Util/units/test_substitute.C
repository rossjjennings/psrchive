/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "substitute.h"

#include <iostream>
using namespace std;

class tester {

public:
  string get_value (string A) const {
    std::transform (A.begin(), A.end(), A.begin(), ::toupper);
    return A;
  };

};

int main ()
{
  cerr << "testing substitute; this should finish almost instantly" << endl;

  tester test;

  string input = " some \t text $with a couple of $subst1_tutions:in it";

  string output = substitute (input, &test);

  if (output != " some \t text WITH a couple of SUBST1_TUTIONS:in it" ) {
    cerr << "substitute 1 failed;\noutput=" << output << endl;
    return -1;
  }

  output = substitute ("$input", &test);
  if (output != "INPUT") {
    cerr << "substitute 2 failed;\noutput=" << output << endl;
    return -1;
  }

  input = "no substitution";

  output = substitute (input, &test);
  if (output != input) {
    cerr << "substitute 3 failed;\noutput=" << output << endl;
    return -1;
  }

  cerr << "substitute function passes all tests" << endl;

  return 0;
}
