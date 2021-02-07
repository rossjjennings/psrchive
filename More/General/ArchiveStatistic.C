/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ArchiveStatistic.h"
#include "Pulsar/ProfileStatistic.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include "BinaryStatistic.h"
#include "ndArray.h"

#include <algorithm>
#include <cassert>

using namespace std;
using namespace Pulsar;

class ProfileStatisticWrapper : public Identifiable::Proxy<ArchiveStatistic>
{
  Reference::To<ProfileStatistic> stat;
  
public:

  ProfileStatisticWrapper (ProfileStatistic* my_stat)
  : Identifiable::Proxy<ArchiveStatistic> (my_stat)
  {
    stat = my_stat;
  }

  double get ()
  {
    return stat->get( get_Profile() );
  }

  TextInterface::Parser* get_interface () { return 0; }

  ProfileStatisticWrapper* clone () const 
  { return new ProfileStatisticWrapper(*this); }
};

class BinaryStatisticSummary : public Identifiable::Proxy<ArchiveStatistic>
{
  Reference::To<BinaryStatistic> stat;
  bool built;

  ndArray<2,double> result;
  ndArray<2,double> temp;

  Reference::To<UnaryStatistic> summary;
  
public:

  BinaryStatisticSummary (BinaryStatistic* my_stat)
    : Identifiable::Proxy<ArchiveStatistic> (my_stat)
  {
    stat = my_stat;
    built = false;

    // by default, sum over polarizations
    set_pol (Index(0, true));

    summary = UnaryStatistic::factory ("median");
  }

  void set_Archive (const Archive* arch)
  {
    if (!archive || archive != arch)
    {
      built = false;
      HasArchive::set_Archive (arch);
    }
  }
  
  double get ()
  {
    if (!built)
      build ();

    unsigned isubint = get_subint().get_value();
    unsigned ichan = get_chan().get_value();
    
    return result[isubint][ichan];
  }

  void build ()
  {
    const Archive* arch = get_Archive();
    unsigned nsubint = arch->get_nsubint();
    unsigned nchan = arch->get_nsubint();

    result * nsubint * nchan;
    temp * nsubint * nsubint;
    
    for (unsigned ichan=0; ichan < nchan; ichan++)
    {
      set_chan (ichan);
      
      for (unsigned isubint=0; isubint < nsubint; isubint++)
      {
	set_subint (isubint);
	
	Reference::To<const Profile> iprof = get_Profile ();

	if (iprof->get_weight() == 0.0)
	{
	  result[isubint][ichan] = 0.0;
	  continue;
	}
	
	vector<double> idata (iprof->get_amps(),
			      iprof->get_amps() + iprof->get_nbin());
	
	for (unsigned jsubint=isubint+1; jsubint < nsubint; jsubint++)
	{
	  set_subint (jsubint);
	  Reference::To<const Profile> jprof = get_Profile ();

	  if (jprof->get_weight() == 0.0)
	  {
	    temp[isubint][jsubint] = temp[jsubint][isubint] = 0.0;
	    continue;
	  }

	  vector<double> jdata (jprof->get_amps(),
				jprof->get_amps() + jprof->get_nbin());

	  double val = stat->get (idata, jdata);

	  temp[isubint][jsubint] = temp[jsubint][isubint] = val;
	}

	std::vector<double> data (nsubint-1);
	unsigned ndat = 0;
	
	for (unsigned jsubint=0; jsubint < nsubint; jsubint++)
	{
	  if (jsubint == isubint)
	    continue;

	  if (temp[isubint][jsubint] == 0.0)
	    continue;
	  
	  data[ndat] = temp[isubint][jsubint];
	  ndat ++;
	}

	if (ndat == 0)
	  result[isubint][ichan] = 0.0;
	else
	{
	  data.resize (ndat);
	  result[isubint][ichan] = summary->get(data);
	}
      }
    }

    built = true;
  }
  
  class Interface : public TextInterface::To<BinaryStatisticSummary>
  {
  public:

    //! Default constructor
    Interface ( BinaryStatisticSummary* stats )
      {
	if (instance)
	  set_instance (instance);

      }
  };
  
  TextInterface::Parser* get_interface () { return new Interface (this); }

  BinaryStatisticSummary* clone () const
  {
    return new BinaryStatisticSummary(*this);
  }
};

static unsigned instance_count = 0;

Pulsar::ArchiveStatistic::ArchiveStatistic ()
{
  instance_count ++;
}

Pulsar::ArchiveStatistic::ArchiveStatistic (const string& name, 
                                            const string& description)
{
  instance_count ++;

  set_identity (name);
  set_description (description);
}

#include "identifiable_factory.h"

static std::vector< Pulsar::ArchiveStatistic* >* instances = NULL;

void Pulsar::ArchiveStatistic::build ()
{
  // ThreadContext::Lock lock (context);

  if (instances != NULL)
    return;

  // cerr << "Pulsar::ArchiveStatistic::build" << endl;
 
  instances = new std::vector< ArchiveStatistic* >;
 
  unsigned start_count = instance_count;
 
  auto profile_statistics = ProfileStatistic::children ();
  for (auto element : profile_statistics)
    instances->push_back( new ProfileStatisticWrapper (element) );

  auto binary_statistics = BinaryStatistic::children ();
  for (auto element : binary_statistics)
    instances->push_back( new BinaryStatisticSummary (element) );

  assert (instances->size() == instance_count - start_count);

  // cerr << "Pulsar::ArchiveStatistic::build instances=" << instances << endl;
}


Pulsar::ArchiveStatistic*
Pulsar::ArchiveStatistic::factory (const std::string& name)
{
  // cerr << "Pulsar::ArchiveStatistic::factory instances=" << instances << endl;

  if (instances == NULL)
    build ();

  assert (instances != NULL);

  // cerr << "ArchiveStatistic::factory instances=" << instances << endl;

  ArchiveStatistic* stat = 0;
  stat = identifiable_factory<ArchiveStatistic> (*instances, name);

  // cerr << "Pulsar::ArchiveStatistic::factory return=" << stat << endl;
  return stat;
}

#include "interface_stream.h"

namespace Pulsar
{
  std::ostream& operator<< (std::ostream& ostr,
                            ArchiveStatistic* stat)
  {
    return interface_insertion (ostr, stat);
  }

  std::istream& operator>> (std::istream& istr,
                            ArchiveStatistic* &stat)
  {
    return interface_extraction (istr, stat);
  }
}

