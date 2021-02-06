/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "BinaryStatistic.h"
#include "FTransform.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>

using namespace std;

namespace BinaryStatistics {

  class Maximum : public BinaryStatistic
  {
  public:
    Maximum ()
      : BinaryStatistic ("max", "maximum value")
      {
        add_alias ("maximum");
      }

    double get (const vector<double>& data, const vector<double>& too)
    {
      return *std::max_element (data.begin(), data.end());
    }

    Maximum* clone () const { return new Maximum(*this); }

  };


} // namespace BinaryStatistics

static unsigned instance_count = 0;

BinaryStatistic::BinaryStatistic (const string& name, 
					const string& description)
{
  instance_count ++;

  set_identity (name);
  set_description (description);
}

#include "identifiable_factory.h"

static std::vector< BinaryStatistic* >* instances = NULL;

using namespace BinaryStatistics;

void BinaryStatistic::build ()
{
  // ThreadContext::Lock lock (context);

  if (instances != NULL)
    return;

  // cerr << "BinaryStatistic::build" << endl;
 
  instances = new std::vector< BinaryStatistic* >;
 
  unsigned start_count = instance_count;
 
  instances->push_back( new Maximum );

  // cerr << "BinaryStatistic::build instances=" << instances << " count=" << instance_count << " start=" << start_count << " size=" << instances->size() << endl; 

  assert (instances->size() == instance_count - start_count);

}


const std::vector< BinaryStatistic* >& BinaryStatistic::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

BinaryStatistic*
BinaryStatistic::factory (const std::string& name)
{
  return identifiable_factory<BinaryStatistic> (children(), name);
}

