/***************************************************************************
 *
 *   Copyright (C) 2011 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "MEAL/ScalarVector.h"

MEAL::ScalarVector::ScalarVector ()
{
  index = 0;
}

//! Set the index of the position angle array
void MEAL::ScalarVector::set_index (unsigned i)
{
  if (index != i)
    set_evaluation_changed ();

  index = i;
}

//! Get the index of the position angle array
unsigned MEAL::ScalarVector::get_index () const
{
  return index;
}
