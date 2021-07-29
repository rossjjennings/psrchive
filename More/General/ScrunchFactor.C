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

const ScrunchFactor ScrunchFactor::none (0);

std::istream& Pulsar::operator >> (std::istream& is, ScrunchFactor& factor)
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

std::ostream& Pulsar::operator << (std::ostream& os, const ScrunchFactor& factor)
{
  if (factor.get_nresult())
    os << factor.get_nresult();
  else
    os << "x" << factor.get_nscrunch();

  return os;
}

unsigned Pulsar::ScrunchFactor::get_nresult (unsigned size) const
{
  if (n_result)
    return n_result;
  else if (n_scrunch)
    return size / n_scrunch;
  else
    return size;
}

unsigned Pulsar::ScrunchFactor::get_nscrunch (unsigned size) const
{
  if (n_scrunch)
    return n_scrunch;
  else if (n_result)
    return size / n_result;
  else
    return 1;
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

