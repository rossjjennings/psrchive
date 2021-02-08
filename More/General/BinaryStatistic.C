/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "BinaryStatistic.h"
#include "UnaryStatistic.h"
#include "FTransform.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>

using namespace std;

namespace BinaryStatistics {

  class CrossCorrelation : public BinaryStatistic
  {
  public:
    CrossCorrelation ()
      : BinaryStatistic ("ccc", "cross-correlation coefficient")
      {
        add_alias ("ccf");
      }

    double get (const vector<double>& dat1, const vector<double>& dat2)
    {
      assert (dat1.size() == dat2.size());
      
      vector<double> mu1 (2);
      central_moments (dat1, mu1);
      // mu1[0] = mean
      // mu1[1] = variance
      
      vector<double> mu2 (2);
      central_moments (dat1, mu2);

      double coeff = 0.0;
      for (unsigned i=0; i<dat1.size(); i++)
	coeff += (dat1[i]-mu1[0]) * (dat2[i]-mu2[0]);

      return coeff / ( dat1.size() * sqrt( mu1[1] * mu2[1] ) );
    }

    CrossCorrelation* clone () const { return new CrossCorrelation(*this); }
  };

  class ChiSquared : public BinaryStatistic
  {
    bool linear_fit;
    
  public:
    ChiSquared ()
      : BinaryStatistic ("chi", "variance of difference")
      {
	linear_fit = true;
      }

    double sqr (double x) { return x*x; }

    double get (const vector<double>& dat1, const vector<double>& dat2)
    {
      assert (dat1.size() == dat2.size());

      double scale = 1.0;
      double offset = 0.0;

      if (linear_fit)
      {
	double covar = 0.0;
	double mu_1 = 0.0;
	double mu_2 = 0.0;
	double var_2 = 0.0;
	
	for (unsigned i=0; i<dat1.size(); i++)
	{
	  mu_1 += dat1[i];
	  mu_2 += dat2[i];
	  var_2 += dat2[i] * dat2[i];
	  covar += dat1[i] * dat2[i];
	}
	
	mu_1 /= dat1.size();
	mu_2 /= dat2.size();
	covar /= dat1.size();
	covar -= mu_1 * mu_2;
	var_2 /= dat2.size();
	var_2 -= mu_2 * mu_2;
	
	scale = covar / var_2;

	for (unsigned i=0; i<dat1.size(); i++)
	  offset += dat1[i] - scale * dat2[i];

	offset /= dat1.size();
      }

      double coeff = 0.0;
      for (unsigned i=0; i<dat1.size(); i++)
	coeff += sqr(dat1[i] - scale * dat2[i] - offset);

      return coeff / ( dat1.size() * ( 1 + sqr(scale) ) );
    }

    ChiSquared* clone () const { return new ChiSquared(*this); }
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
 
  instances->push_back( new CrossCorrelation );
  instances->push_back( new ChiSquared );

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

