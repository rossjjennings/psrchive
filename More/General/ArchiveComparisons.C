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

// #define _DEBUG 1
#include "debug.h"

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

  bscrunch_factor.disable_scrunch();
  
  DEBUG("ArchiveComparisons stat=" << stat->get_identity());
}

void ArchiveComparisons::set_Archive (const Archive* arch)
{
  if (arch == NULL)
    built = false;

  if (archive && archive != arch)
    built = false;

  HasArchive::set_Archive (arch);
}

//! Compute covariance matrix from bscrunched clone of data
void ArchiveComparisons::set_bscrunch (const ScrunchFactor& f)
{
  bscrunch_factor = f;
  built = false;
}

void ArchiveComparisons::set_setup_Archive (const Archive* arch)
{
  DEBUG("ArchiveComparisons::set_setup_Archive arch=" << (void*) arch);
  build_compare ();
  DEBUG("ArchiveComparisons::set_setup_Archive init nchan=" << arch->get_nchan());
  init_compare (arch);
  DEBUG("ArchiveComparisons::set_setup_Archive CompareWith::set_setup_data");
  HasArchive::set_Archive (arch);
  compare->set_setup_data (arch);
  DEBUG("ArchiveComparisons::set_setup_Archive done");
  built = false;
}

bool ArchiveComparisons::get_setup ()
{
  return compare->get_setup ();
}

double ArchiveComparisons::get ()
{
  if (!built)
    build ();

  unsigned isubint = get_subint().get_value();
  unsigned ichan = get_chan().get_value();

  DEBUG("ArchiveComparisons::get isub=" << isubint << " ichan=" << ichan << " val=" << result[isubint][ichan]);
  
  return result[isubint][ichan];
}

void ArchiveComparisons::build_compare ()
{
  if (what == "each")
    compare = new CompareWithEachOther;
  else if (what == "sum")
    compare = new CompareWithSum;
  else
    throw Error (InvalidState, "ArchiveComparisons::build_compare",
		 "'what' must be 'each' or 'sum'");
}

void ArchiveComparisons::init_compare (const Archive* arch)
{
  unsigned nsubint = arch->get_nsubint();
  unsigned nchan = arch->get_nchan();

  DEBUG("ArchiveComparisons::init_compare nsubint=" << nsubint << " nchan=" << nchan << " fptr=" << (void*) fptr);

  compare->set_file (fptr);

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
    throw Error (InvalidState, "ArchiveComparisons::init_compare",
		 "way must be 'time' or 'freq' or 'all'");

  DEBUG("ArchiveComparisons::init_compare bscrunch by " << bscrunch_factor);
    
  compare->set_bscrunch (bscrunch_factor);
  compare->set_statistic (stat);
  compare->set_data (this);

  DEBUG("ArchiveComparisons::init_compare return");
}


void ArchiveComparisons::build () try
{
  const Archive* arch = get_Archive();
  unsigned nsubint = arch->get_nsubint();
  unsigned nchan = arch->get_nchan();

  DEBUG("ArchiveComparisons::build nsubint=" << nsubint << " nchan=" << nchan);

  result * nsubint * nchan;
  
  if (!compare)
    build_compare ();

  init_compare (arch);
  
  Index isubint_restore = get_subint();
  Index ichan_restore = get_chan();
  Index ipol_restore = get_pol();

  DEBUG("ArchiveComparisons::build call CompareWith::compute");

  compare->compute (result);

  DEBUG("ArchiveComparisons::build CompareWith::compute returned");

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
  string description;
  
 public:
  
  std::string get_interface_name () const { return name; }
  std::string get_interface_description () const { return description; }
 
  //! Default constructor
  Interface ( ArchiveComparisons* _instance )
  {
    if (_instance)
      set_instance (_instance);
    
    name = _instance->get_identity ();
    description = _instance->get_description ();

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
