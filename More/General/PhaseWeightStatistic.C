/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/PhaseWeightStatistic.h"
#include "Pulsar/PhaseWeight.h"
#include "UnaryStatistic.h"

#include <algorithm>
#include <cassert>
#include <cmath>

using namespace std;
using namespace Pulsar;

static unsigned instance_count = 0;

Pulsar::PhaseWeightStatistic::PhaseWeightStatistic (UnaryStatistic* my_stat) :
  Identifiable::Decorator (my_stat)
{
  instance_count ++;
  stat = my_stat;
}

double Pulsar::PhaseWeightStatistic::get (const PhaseWeight* prof)
{
  vector<float> tmp;
  prof->get_filtered (tmp, false, 0.0);

  if (tmp.size() == 0)
    return nan("");

  vector<double> data (tmp.begin(), tmp.end());
  return stat->get (data);
}

Pulsar::PhaseWeightStatistic* Pulsar::PhaseWeightStatistic::clone () const
{
  return new PhaseWeightStatistic (*this);
}

#include "identifiable_factory.h"

static std::vector< Pulsar::PhaseWeightStatistic* >* instances = NULL;

void Pulsar::PhaseWeightStatistic::build ()
{
  // ThreadContext::Lock lock (context);

  if (instances != NULL)
    return;

  // cerr << "Pulsar::PhaseWeightStatistic::build" << endl;
 
  instances = new std::vector< PhaseWeightStatistic* >;
 
  unsigned start_count = instance_count;

  auto unary_statistics = UnaryStatistic::children ();

  for (auto element : unary_statistics)
    instances->push_back( new PhaseWeightStatistic (element) );

  assert (instances->size() == instance_count - start_count);
}

const std::vector<Pulsar::PhaseWeightStatistic*>& 
Pulsar::PhaseWeightStatistic::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

Pulsar::PhaseWeightStatistic*
Pulsar::PhaseWeightStatistic::factory (const std::string& name)
{
  return identifiable_factory<PhaseWeightStatistic> (children(), name);
}

