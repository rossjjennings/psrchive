/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarVector.h"

using namespace std;

MEAL::ScalarVector::ScalarVector ()
{
  index = 0;
}

//! Set the index of the position angle array
void MEAL::ScalarVector::set_index (unsigned i)
{
  if (index != i)
    set_evaluation_changed ();

  // cerr << "MEAL::ScalarVector::set_index index=" << i << endl;
  index = i;
}

//! Get the index of the position angle array
unsigned MEAL::ScalarVector::get_index () const
{
  // cerr << "MEAL::ScalarVector::get_index index=" << index << endl;
  return index;
}
