/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "polyco.h"
#include "T2Predictor.h"
// #include "Pulsar/SimplePredictor.h"

using namespace std;

void Pulsar::Predictor::children (vector< Reference::To<Predictor> >& child)
{
  child.resize (0);

  /*
    polyco should come first because the Tempo2::Predictor will
    successfully load a polyco, but the code differentiates between
    the two based on class type 
  */

  child.push_back (new polyco);
  child.push_back (new Tempo2::Predictor);
  // child.push_back (new Pulsar::SimplePredictor);
}
