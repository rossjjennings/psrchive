/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
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
  {
    return stat->get( get_Profile() );
  }

  class Interface : public TextInterface::To<ProfileStatisticWrapper>
  {
    string name;
    string description;

  public:

    std::string get_interface_name () const { return name; }
    std::string get_interface_description () const { return description; }

    //! Default constructor
    Interface ( ProfileStatisticWrapper* _instance )
      {
        if (_instance)
          set_instance (_instance);

        name = _instance->get_identity ();
        description = _instance->get_description ();
      }
  };

  TextInterface::Parser* get_interface () { return new Interface(this); }

  ProfileStatisticWrapper* clone () const 
  { return new ProfileStatisticWrapper(*this); }
};


static unsigned instance_count = 0;

Pulsar::ArchiveStatistic::ArchiveStatistic ()
{
  instance_count ++;
  fptr = 0;
}

Pulsar::ArchiveStatistic::ArchiveStatistic (const string& name, 
                                            const string& description)
{
  instance_count ++;
  fptr = 0;
  
  set_identity (name);
  set_description (description);
}

void Pulsar::ArchiveStatistic::fclose ()
{
  if (fptr) ::fclose (fptr); fptr = 0;
}

#include "Pulsar/ArchiveComparisons.h"
#include "BinaryStatistic.h"

static std::vector< Pulsar::ArchiveStatistic* >* instances = NULL;

using namespace Pulsar;

static void instances_build ()
{
  // ThreadContext::Lock lock (context);

  if (instances != NULL)
    return;

  // cerr << "Pulsar::ArchiveStatistic::build" << endl;
 
  instances = new std::vector< Pulsar::ArchiveStatistic* >;
 
  unsigned start_count = instance_count;
 
  auto profile_statistics = ProfileStatistic::children ();
  for (auto element : profile_statistics)
    instances->push_back( new ProfileStatisticWrapper (element) );

  auto binary_statistics = BinaryStatistic::children ();
  for (auto element : binary_statistics)
    instances->push_back( new ArchiveComparisons (element) );

  assert (instances->size() == instance_count - start_count);

  // cerr << "Pulsar::ArchiveStatistic::build instances=" << instances << endl;
}

#include "interface_factory.h"

Pulsar::ArchiveStatistic*
Pulsar::ArchiveStatistic::factory (const std::string& name)
{
  // cerr << "Pulsar::ArchiveStatistic::factory instances=" << instances << endl;

  if (instances == NULL)
    instances_build ();

  assert (instances != NULL);

  // cerr << "ArchiveStatistic::factory instances=" << instances << endl;

  ArchiveStatistic* stat = 0;
  stat = TextInterface::factory<ArchiveStatistic> (*instances, name);

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

