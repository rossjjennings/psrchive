/***************************************************************************
 *
 *   Copyright (C) 2020 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

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
      central_moments (dat2, mu2);

      double coeff = 0.0;
      for (unsigned i=0; i<dat1.size(); i++)
	coeff += (dat1[i]-mu1[0]) * (dat2[i]-mu2[0]);

      return coeff / ( dat1.size() * sqrt( mu1[1] * mu2[1] ) );
    }

    CrossCorrelation* clone () const { return new CrossCorrelation; }
  };

  //! based on Kullback-Leibler divergence
  class RelativeSpectralEntropy : public BinaryStatistic
  {
    double threshold;
    
  public:
    RelativeSpectralEntropy ()
      : BinaryStatistic ("rse", "relative spectral entropy")
      {
	// exponential distribution has a long tail
	threshold = 9.0; // sigma

        add_alias ("kld");  // Kullback-Leibler divergence
      }

    double get (const vector<double>& dat1, const vector<double>& dat2)
    {
      /* robust_variance returns the variance of dat1; which is the
	 standard deviation of the exponentially distributed PSD */
      vector<float> fps1;
      double cut1 = robust_variance (dat1, &fps1) * threshold;

      vector<float> fps2;
      double cut2 = robust_variance (dat2, &fps2) * threshold;

      unsigned istart = 1; // skip DC
      double sum1 = 0.0;
      double sum2 = 0.0;
      unsigned count = 0;
      
      for (unsigned i=istart; i < fps1.size(); i++)
      {
	if (fps1[i] < cut1 || fps2[i] < cut2)
	  continue;
      
	sum1 += fps1[i];
	sum2 += fps2[i];
	count ++;
      }

      cerr << "count=" << count << " sum1/sum2=" << sum1/sum2 << endl;
      
      double relative_entropy = 0;
      for (unsigned i=istart; i < fps1.size(); i++)
      {
	if (fps1[i] < cut1 || fps2[i] < cut2)
	  continue;
	
	double p = fps1[i] / sum1;
	double q = fps2[i] / sum2;

	relative_entropy += p * abs(log(p/q));
      }
    
      return relative_entropy;
    }

    RelativeSpectralEntropy* clone () const
    { return new RelativeSpectralEntropy; }
  };
  
} // namespace BinaryStatistics

static unsigned instance_count = 0;

BinaryStatistic::BinaryStatistic (const string& name, 
				  const string& description)
{
  instance_count ++;

  set_identity (name);
  set_description (description);

  fptr = 0;
}

static std::vector< BinaryStatistic* >* instances = NULL;

#include "ChiSquared.h"
#include "GeneralizedChiSquared.h"

#if HAVE_ARMADILLO
#include "GaussianMixtureProbabilityDensity.h"
#endif

using namespace BinaryStatistics;

void BinaryStatistic::build ()
{
  if (instances != NULL)
    return;

  instances = new std::vector< BinaryStatistic* >;
 
  unsigned start_count = instance_count;
 
  instances->push_back( new CrossCorrelation );
  instances->push_back( new ChiSquared );
  instances->push_back( new GeneralizedChiSquared );
  instances->push_back( new RelativeSpectralEntropy );

#if HAVE_ARMADILLO
  instances->push_back( new GaussianMixtureProbabilityDensity );
#endif
  
  assert (instances->size() == instance_count - start_count);
}

const std::vector< BinaryStatistic* >& BinaryStatistic::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

#include "identifiable_factory.h"

BinaryStatistic*
BinaryStatistic::factory (const std::string& name)
{
  return identifiable_factory<BinaryStatistic> (children(), name);
}

