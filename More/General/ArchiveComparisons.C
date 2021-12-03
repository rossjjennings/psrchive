/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveComparisons.h"

#include "Pulsar/CompareWithEachOther.h"
#include "Pulsar/CompareWithSum.h"

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

  what = "each";
  way = "time";
  
#if _DEBUG
  cerr << "ArchiveComparisons my_stat=" << stat->get_identity()
       << " summary=" << summary->get_identity() << endl;
#endif
}

void ArchiveComparisons::set_Archive (const Archive* arch)
{
  if (arch == NULL)
    built = false;

  if (archive && archive != arch)
    built = false;

  HasArchive::set_Archive (arch);
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

#if _DEBUG
  cerr << "ArchiveComparisons::build nsubint=" << nsubint 
       << " nchan=" << nchan << endl;
#endif

  result * nsubint * nchan;
  
  Reference::To<CompareWith> compare;
  
  if (what == "each")
    compare = new CompareWithEachOther;
  else if (what == "sum")
    compare = new CompareWithSum;
  else
    throw Error (InvalidState, "ArchiveComparisons::build",
		 "'what' must be 'each' or 'sum'");

  if (way == "time")
  {
    compare->set_primary (nchan, &HasArchive::set_chan);
    compare->set_compare (nsubint, &HasArchive::set_subint);
    compare->set_transpose (true);
    compare->set_compute_mask (compute_chan);
  }
  else if ((way == "freq") || (way == "all"))
  {
    compare->set_primary (nsubint, &HasArchive::set_subint);
    compare->set_compare (nchan, &HasArchive::set_chan);
    compare->set_transpose (false);
    if (way == "all")
      compare->set_compare_all ();
    else
      compare->set_compute_mask (compute_subint);
  }
  else
    throw Error (InvalidState, "ArchiveComparisons::build",
		 "way must be 'time' or 'freq' or 'all'");

  Index isubint_restore = get_subint();
  Index ichan_restore = get_chan();
  Index ipol_restore = get_pol();

  compare->set_statistic (stat);
  compare->set_data (this);
  compare->compute (result);

  set_subint (isubint_restore);
  set_chan (ichan_restore);
  set_pol (ipol_restore);
  
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

    add( &ArchiveComparisons::get_what,
	 &ArchiveComparisons::set_what,
	 "what", "'each' or 'sum'" );

    add( &ArchiveComparisons::get_way,
	 &ArchiveComparisons::set_way,
	 "way", "'time' or 'freq'" );
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
