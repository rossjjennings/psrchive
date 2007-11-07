/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Instrument.h"
#include "MEAL/Polynomial.h"

using namespace std;
using namespace MEAL;
using namespace Calibration;

void banner (const char* text)
{
  cerr << endl
       << "******************************************************************"
       << endl << endl
       << text 
       << endl << endl
       << "******************************************************************"
       << endl << endl;
}

int main () try {

  Function::verbose = true;
  Function::very_verbose = true;

  banner ("constructing instrument");

  Instrument instrument;

  unsigned nparam = instrument.get_nparam();

  banner ("set gain to polynomial");

  instrument.set_gain( new Polynomial(3) );

  if (instrument.get_nparam() != nparam + 3) {
    cerr << "Chain Rule error map" << endl;
    return -1;
  }

  Polynomial* zero = 0;

  banner ("set gain to zero");

  instrument.set_gain( zero );

  if (instrument.get_nparam() != nparam) {
    cerr << "Chain Rule error unmap" << endl;
    return -1;
  }

  banner ("nested Composite map/unmap test passed");

  return 0;
}
 catch (Error& error) {
   cerr << error << endl;
   return -1;
 }
