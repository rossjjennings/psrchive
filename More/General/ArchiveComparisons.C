/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveComparisons.h"
#include "Pulsar/CompareWithEachOther.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include "BinaryStatistic.h"
#include "UnaryStatistic.h"

#include <algorithm>
#include <cassert>

using namespace std;
using namespace Pulsar;

ArchiveComparisons::ArchiveComparisons (BinaryStatistic* my_stat)
: Identifiable::Proxy<ArchiveStatistic> (my_stat)
{
  stat = my_stat;
  built = false;
  
  // by default, sum over polarizations
  set_pol (Index(0, true));

#if _DEBUG
  cerr << "ArchiveComparisons my_stat=" << stat->get_identity()
       << " summary=" << summary->get_identity() << endl;
#endif
}

void ArchiveComparisons::set_Archive (const Archive* arch)
{
  if (!archive || archive != arch)
  {
    built = false;
    HasArchive::set_Archive (arch);
  }
}
  
double ArchiveComparisons::get ()
{
  if (!built)
    build ();

  unsigned isubint = get_subint().get_value();
  unsigned ichan = get_chan().get_value();

#if _DEBUG
  cerr << "ArchiveComparisons::get isub=" << isubint << " ichan=" << ichan
       << " val=" << result[isubint][ichan] << endl;
#endif
  
  return result[isubint][ichan];
}

void ArchiveComparisons::build () try
{
  const Archive* arch = get_Archive();
  unsigned nsubint = arch->get_nsubint();
  unsigned nchan = arch->get_nchan();
  
  result * nsubint * nchan;
  
  CompareWithEachOther* compare = new CompareWithEachOther;
  
  compare->set_statistic (stat);
  compare->set_data (this);
  compare->set_primary (nchan, &HasArchive::set_chan);
  compare->set_compare (nsubint, &HasArchive::set_subint);
  compare->set_transpose (true);
  
  compare->compute (result);
  
  built = true;
}
catch (Error& error)
{
  cerr << error << endl;
}

class ArchiveComparisons::Interface
: public TextInterface::To<ArchiveComparisons>
{
  string name;
  
 public:
  
  std::string get_interface_name () const { return name; }
  
  //! Default constructor
  Interface ( ArchiveComparisons* _instance )
  {
    if (_instance)
      set_instance (_instance);
    
    name = _instance->get_identity ();
  }
};

TextInterface::Parser* ArchiveComparisons::get_interface ()
{
  return new Interface (this);
}

ArchiveComparisons* ArchiveComparisons::clone () const
{
  return new ArchiveComparisons(*this);
}
