/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatistic.h"
#include "Pulsar/Profile.h"
#include "UnaryStatistic.h"

#include <algorithm>
#include <cassert>

using namespace std;
using namespace Pulsar;

static unsigned instance_count = 0;

Pulsar::ProfileStatistic::ProfileStatistic (UnaryStatistic* my_stat) :
  Identifiable::Decorator (my_stat)
{
  instance_count ++;
  stat = my_stat;
}

double Pulsar::ProfileStatistic::get (const Profile* prof)
{
  vector<double> data (prof->get_amps(), prof->get_amps() + prof->get_nbin());
  return stat->get (data);
}

Pulsar::ProfileStatistic* Pulsar::ProfileStatistic::clone () const
{
  return new ProfileStatistic (*this);
}

#include "identifiable_factory.h"

static std::vector< Pulsar::ProfileStatistic* >* instances = NULL;

void Pulsar::ProfileStatistic::build ()
{
  // ThreadContext::Lock lock (context);

  if (instances != NULL)
    return;

  // cerr << "Pulsar::ProfileStatistic::build" << endl;
 
  instances = new std::vector< ProfileStatistic* >;
 
  unsigned start_count = instance_count;

  auto unary_statistics = UnaryStatistic::children ();

  for (auto element : unary_statistics)
    instances->push_back( new ProfileStatistic (element) );

  assert (instances->size() == instance_count - start_count);
}

const std::vector<Pulsar::ProfileStatistic*>& 
Pulsar::ProfileStatistic::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

Pulsar::ProfileStatistic*
Pulsar::ProfileStatistic::factory (const std::string& name)
{
  return identifiable_factory<ProfileStatistic> (children(), name);
}

