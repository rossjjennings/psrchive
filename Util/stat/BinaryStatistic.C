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

void linear_fit (double& scale, double& offset,
		 const vector<double>& dat1, const vector<double>& dat2,
		 const vector<bool>* mask)
{
  double covar = 0.0;
  double mu_1 = 0.0;
  double mu_2 = 0.0;
  double var_2 = 0.0;
  unsigned count = 0;
    
  for (unsigned i=0; i<dat1.size(); i++)
  {
    if (mask && !(*mask)[i])
      continue;
      
    mu_1 += dat1[i];
    mu_2 += dat2[i];
    var_2 += dat2[i] * dat2[i];
    covar += dat1[i] * dat2[i];
    
    count ++;
  }
	
  mu_1 /= count;
  mu_2 /= count;
  covar /= count;
  covar -= mu_1 * mu_2;
  var_2 /= count;
  var_2 -= mu_2 * mu_2;
  
  scale = covar / var_2;
  
  vector<double> diff (count);
  unsigned idiff = 0;
  
  for (unsigned i=0; i<dat1.size(); i++)
  {
    if (mask && !(*mask)[i])
      continue;
    
    diff[idiff] = dat1[i] - scale * dat2[i];
    idiff ++;
  }
  
  assert (idiff == count);
  
  offset = median (diff);
}


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

  static double sqr (double x) { return x*x; }

  class ChiSquared : public BinaryStatistic
  {
    bool robust_linear_fit;
    double max_zap_fraction;
 
  public:
    ChiSquared ()
      : BinaryStatistic ("chi", "variance of difference")
      {
	robust_linear_fit = true;
        max_zap_fraction = 0.5;
      }

    double get (const vector<double>& dat1, const vector<double>& dat2)
    {
      assert (dat1.size() == dat2.size());
      
      double scale = 1.0;
      double offset = 0.0;

      if (robust_linear_fit)
      {
	vector<bool> mask (dat1.size(), true);

	unsigned total_zapped = 0;
        unsigned max_zapped = dat1.size();
        if (max_zap_fraction)
	  max_zapped = max_zap_fraction * max_zapped;

        unsigned zapped = 0;
	do
	{
	  linear_fit (scale, offset, dat1, dat2, &mask);

	  double sigma = 2.0;
	  double var = 1 + sqr(scale);
	  double cut = sqr(sigma) * var;

	  zapped = 0;
	  
	  for (unsigned i=0; i<dat1.size(); i++)
	  {
	    if (!mask[i])
	      continue;
		
	    double residual = dat1[i] - scale * dat2[i] - offset;
	    if ( sqr(residual) > cut )
	    {
	      mask[i] = false;
	      zapped ++;
	    }
	  }

	  total_zapped += zapped;
	}
        while (zapped && total_zapped < max_zapped);
      }
      
      double coeff = 0.0;
      for (unsigned i=0; i<dat1.size(); i++)
	coeff += sqr(dat1[i] - scale * dat2[i] - offset);

      return coeff / ( dat1.size() * ( 1 + sqr(scale) ) );
    }

    ChiSquared* clone () const { return new ChiSquared; }
  };

  //! based on Kullback-Leibler divergence
  class RelativeSpectralEntropy : public BinaryStatistic
  {
    double threshold;
    
  public:
    RelativeSpectralEntropy ()
      : BinaryStatistic ("rse", "Relative Spectral Entropy")
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
}

static std::vector< BinaryStatistic* >* instances = NULL;

using namespace BinaryStatistics;

void BinaryStatistic::build ()
{
  if (instances != NULL)
    return;

  instances = new std::vector< BinaryStatistic* >;
 
  unsigned start_count = instance_count;
 
  instances->push_back( new CrossCorrelation );
  instances->push_back( new ChiSquared );
  instances->push_back( new RelativeSpectralEntropy );

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

