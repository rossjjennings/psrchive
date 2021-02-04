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

class Maxibum : public ArchiveStatistic
{     
public:
  Maxibum ()
  : ArchiveStatistic ("max", "maximum profile amplitude")
  {
    add_alias ("maximum");
  }

  double get ()
  { return get_Profile()->max(); }

  Maxibum* clone () const { return new Maxibum(*this); }

};

class ProfileStatisticWrapper : public ArchiveStatistic
{
  Reference::To<ProfileStatistic> stat;

public:
  ProfileStatisticWrapper ()
  : ArchiveStatistic ("wrap", "wraps a profile statistic")
  {
  }

  bool identify (const std::string& name) try
  {
    stat = ProfileStatistic::factory (name);
    return true;
  }
  catch (...)
  {
    return false;
  }

  //! Returns the description of the object
  std::string get_description () const try
  {
    ProfileStatistic::factory ("help");
    return "unexpected";
  }
  catch (Error& error)
  {
    return error.get_message();
  }

  double get ()
  { return stat->get( get_Profile() ); }

  ProfileStatisticWrapper* clone () const 
  { return new ProfileStatisticWrapper(*this); }

};

static unsigned instance_count = 0;

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
 
  instances->push_back( new Maxibum );
  instances->push_back( new ProfileStatisticWrapper );

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

