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
  { return stat->get( get_Profile() ); }

  TextInterface::Parser* get_interface () { return 0; }

  ProfileStatisticWrapper* clone () const 
  { return new ProfileStatisticWrapper(*this); }
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

