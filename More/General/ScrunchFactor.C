/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ScrunchFactor.h"
#include "Error.h"

using namespace std;
using namespace Pulsar;

std::istream& operator >> (std::istream& is, ScrunchFactor& factor)
{
  unsigned temp;

  if (is.peek() == 'x')
  {
    is.get();
    is >> temp;
    factor.set_nscrunch (temp);
  }
  else
  {
    is >> temp;
    factor.set_nresult (temp);
  }

  return is;
}

std::ostream& operator << (std::ostream& os, const ScrunchFactor& factor)
{
  if (factor.get_nresult())
    os << factor.get_nresult();
  else
    os << "x" << factor.get_nscrunch();

  return os;
}

#if 0

#include "Pulsar/Archive.h"

static void test ()
{
  Archive* archive = 0;
  ScrunchFactor factor;

  fscrunch (archive, factor);
  tscrunch (archive, factor);
  bscrunch (archive, factor);
}

#endif

