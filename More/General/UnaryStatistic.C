/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "UnaryStatistic.h"
#include "FTransform.h"

#include <algorithm>
#include <numeric>

#include <cassert>
#include <cmath>

using namespace std;

// worker function for variance, skewness, kurtosis, etc.
void central_moments (vector<double> data, vector<double>& mu)
{
  if (mu.size() == 0)
    return;

  vector<double> prod (data.size(), 1.0);

  for (unsigned i=0; i < mu.size(); i++)
  {
    double total = 0.0;

    for (unsigned j=0; j < data.size(); j++)
    {
      prod[j] *= data[j];
      total += prod[j];
    }

    mu[i] = total / data.size();

    if (i == 0)
    {
      // subract the mean so that further moments are central
      for (unsigned j=0; j < data.size(); j++)
      {
        prod[j] -= mu[0];
        data[j] = prod[j];
      }
    }
  }
}

namespace UnaryStatistics {

  class Maximum : public UnaryStatistic
  {     
  public:
    Maximum ()
      : UnaryStatistic ("max", "maximum value")
      {
	add_alias ("maximum");
      }
    
    double get (const vector<double>& data)
    {
      return *std::max_element (data.begin(), data.end());
    }
    
    Maximum* clone () const { return new Maximum(*this); }
    
  };
  
  class Minimum : public UnaryStatistic
  {
  public:
    Minimum ()
      : UnaryStatistic ("min", "minimum value")
      {
	add_alias ("minimum");
      }
    
    double get (const vector<double>& data)
    {
      return *std::min_element (data.begin(), data.end());
    }
    
    
    Minimum* clone () const { return new Minimum(*this); }
    
  };

  class Range : public UnaryStatistic
  {
  public:
    Range ()
      : UnaryStatistic ("range", "range of values")
      {
	// cerr << "Range ctor this=" << this << endl;
	add_alias ("peak-to-peak");
      }
    
    // ~Range () { cerr << "Range dtor this=" << this << endl; }
    
    double get (const vector<double>& data)
    {
      // C++11
      auto ret = std::minmax_element (data.begin(), data.end());
      return *ret.second - *ret.first;
    }
    
    Range* clone () const { return new Range(*this); }
  };
  
  class Sum : public UnaryStatistic
  {
  public:
    Sum () : UnaryStatistic ("sum", "sum of values")
      {
	add_alias ("total");
      }
    
    double get (const vector<double>& data)
    {
      return std::accumulate (data.begin(), data.end(), 0.0);
    }
    
    Sum* clone () const { return new Sum(*this); }
  };
  
  class Mean : public UnaryStatistic
  {
  public:
    Mean () : UnaryStatistic ("avg", "average of values")
      {
	add_alias ("average");
	add_alias ("mean");
      }
    
    double get (const vector<double>& data)
    {
      return std::accumulate (data.begin(), data.end(), 0.0) / data.size();
    }
    
    Mean* clone () const { return new Mean(*this); }
  };
  
  class StandardDeviation : public UnaryStatistic
  {
  public:
    StandardDeviation () 
      : UnaryStatistic ("rms", "standard deviation of values")
      {
	add_alias ("stddev");
	add_alias ("sigma");
      }
    
    double get (const vector<double>& data)
    {
      vector<double> mu (2);
      central_moments (data, mu);
      return std::sqrt( mu[1] );
    }
    
    StandardDeviation* clone () const { return new StandardDeviation(*this); }
  };


  //! Pearson's moment coefficient of skewness
  class Skewness : public UnaryStatistic
  {
  public:
    Skewness ()
      : UnaryStatistic ("mu3", "skewness of values")
      {
	add_alias ("skewness");
	add_alias ("skew");
      }
    
    double get (const vector<double>& data)
    { 
      vector<double> mu (3);
      central_moments (data, mu);
      
      double sigma = sqrt( mu[1] );
      return mu[2] / pow(sigma,3.0);
    }
    
    Skewness* clone () const { return new Skewness(*this); }
  };
  
  //! the fourth standardized moment
  class Kurtosis : public UnaryStatistic
  {
  public:
    Kurtosis ()
      : UnaryStatistic ("mu4", "kurtosis of values")
      {
	add_alias ("kurtosis");
	add_alias ("kurt");
      }
    
    double get (const vector<double>& data)
    {
      vector<double> mu (4);
      central_moments (data, mu);
      return mu[3] / (mu[1]*mu[1]);
    }
    
    Kurtosis* clone () const { return new Kurtosis(*this); }
  };
  
  // https://en.wikipedia.org/wiki/Coefficient_of_variation
  class DeviationCoefficient : public UnaryStatistic
  {     
  public:
    DeviationCoefficient ()
      : UnaryStatistic ("dev", "coefficient of deviation of values") 
      {
	add_alias ("modulation");
	add_alias ("beta");
	add_alias ("cv");    // coefficient of variation
	add_alias ("rsd");   // relative standard deviation
      }

    double get (const vector<double>& data)
    {
      vector<double> mu (2);
      central_moments (data, mu);
      return sqrt( mu[1] ) / mu[0];
    }
    
    DeviationCoefficient* clone () const
    { return new DeviationCoefficient(*this); }
  };
  
  template<typename T>
    T median (vector<T> data)
    {
      unsigned mid = data.size() / 2;
      std::nth_element( data.begin(), data.begin()+mid, data.end() );
      return data[mid];
    }

  class Median : public UnaryStatistic
  {    
  public:
    Median ()
      : UnaryStatistic ("med", "median of values")
      {
	add_alias ("median");
      }
    
    double get (const vector<double>& data)
    {
      return median (data);
    }
    
    Median* clone () const { return new Median(*this); }
  };
  
  double madm (vector<double> data)
  {
    double med = median (data);

    for (double& element : data)
      element = fabs( element - med );

    return median (data);
  }

class MedianAbsoluteDifference : public UnaryStatistic
{
public:
  MedianAbsoluteDifference ()
  : UnaryStatistic ("mdm", "median absolute difference from median")
  {
    add_alias ("mad");
    add_alias ("madm");
  }

  double get (const vector<double>& data)
  { return madm (data); }

  MedianAbsoluteDifference* clone () const 
  { return new MedianAbsoluteDifference(*this); }

};

void Q1_Q2_Q3 (vector<double> data, double& Q1, double& Q2, double& Q3)
{
  std::sort( data.begin(), data.end() );
  unsigned nbin = data.size();

  unsigned iQ1 = nbin / 4;
  unsigned iQ2 = nbin / 2;
  unsigned iQ3 = (3 * nbin) / 4;

  Q1 = data[iQ1];
  Q2 = data[iQ2];
  Q3 = data[iQ3];
}

double iqr (const vector<double>& data)
{
  double Q1=0, Q2=0, Q3=0;
  Q1_Q2_Q3 (data, Q1, Q2, Q3);
  return Q3 - Q1;
}

class InterQuartileRange : public UnaryStatistic
{
public:
  InterQuartileRange ()
  : UnaryStatistic ("iqr", "inter-quartile range of values")
  {
  }

  double get (const vector<double>& data)
  { return iqr (data); }

  InterQuartileRange* clone () const 
  { return new InterQuartileRange(*this); }

};

double qcd (const vector<double>& data)
{ 
  double Q1=0, Q2=0, Q3=0;
  Q1_Q2_Q3 (data, Q1, Q2, Q3);
  return (Q3 - Q1) / (Q3 + Q1); 
}

// https://en.wikipedia.org/wiki/Quartile_coefficient_of_dispersion
class QuartileDispersion : public UnaryStatistic
{
public:
  QuartileDispersion ()
  : UnaryStatistic ("qcd", "quartile coefficient of dispersion")
  {
  }

  double get (const vector<double>& data)
  { return qcd (data); }

  QuartileDispersion* clone () const
  { return new QuartileDispersion(*this); }
};

double qcs (const vector<double>& data)
{
  double Q1=0, Q2=0, Q3=0;
  Q1_Q2_Q3 (data, Q1, Q2, Q3);
  return (Q3 + Q1 - 2*Q2) / (Q3 - Q1);
}

// https://mathworld.wolfram.com/BowleySkewness.html
class QuartileSkewness: public UnaryStatistic
{
public:
  QuartileSkewness ()
  : UnaryStatistic ("qcs", "quartile coefficient of skewness")
  {
  }

  double get (const vector<double>& data)
  { return qcs (data); }

  QuartileSkewness* clone () const
  { return new QuartileSkewness (*this); }
};

// e.g. N = 4 yields quartiles, N = 10 yields deciles, etc.
void Ntile (vector<double> data, vector<double>& Ntiles)
{
  std::sort( data.begin(), data.end() );
  unsigned nbin = data.size();
  unsigned N = Ntiles.size() + 1;

  for (unsigned i=0; i < Ntiles.size(); i++)
  {
    unsigned itile = (nbin * (i+1)) / N;
    Ntiles[i] = data[itile];
  }
}

class OctileKurtosis: public UnaryStatistic
{
public:
  OctileKurtosis ()
  : UnaryStatistic ("ock", "octile coefficient of kurtosis")
  {
  }

  double get (const vector<double>& data)
  { 
    vector<double> oct (7);
    Ntile (data, oct);

    return ( oct[6]-oct[4] + oct[2]-oct[0] ) / ( oct[5] - oct[1] );
  }

  OctileKurtosis* clone () const
  { return new OctileKurtosis (*this); }
};

void spectrum (const vector<double>& data, vector<float>& spec)
{
  vector<float> copy (data.begin(), data.end());
  spec.resize (data.size() + 2);
  FTransform::frc1d (data.size(), &spec[0], &copy[0]);
}

void power_spectral_density (const vector<double>& data, vector<float>& fps)
{
  vector<float> copy (data.begin(), data.end());
  fps.resize (data.size() + 2);
  
  FTransform::frc1d (data.size(), &fps[0], &copy[0]);

  const unsigned nbin = data.size() / 2 + 1;
  for (unsigned ibin=0; ibin < nbin; ibin++)
  {
    float re = fps[ibin*2];
    float im = fps[ibin*2+1];
    fps[ibin] = re*re + im*im;
  }

  fps.resize (nbin);
  
  if (FTransform::get_norm() == FTransform::unnormalized)
    for (auto& element : fps)
      element /= data.size();
}
    
class FirstHarmonic : public UnaryStatistic
{
public:
  FirstHarmonic ()
  : UnaryStatistic ("ft1", "first harmonic of values")
  {
    add_alias ("fft1");
    add_alias ("fundamental");
  }

  double get (const vector<double>& data)
  {
    vector<float> fourier;
    power_spectral_density (data, fourier);
    return fourier[1];
  }

  FirstHarmonic* clone () const { return new FirstHarmonic(*this); }
};

 double robust_variance (const vector<double>& data, vector<float>* fps = 0)
{
  vector<float> tmp;
  if (fps == 0)
    fps = &tmp;
  
  power_spectral_density (data, *fps);
  vector<double> upper_half (fps->begin() + fps->size()/2, fps->end());

  // divide by log(2) because spectral power has exponential distribution
  return median (upper_half) / log(2.0);
}

class SpectralMedian : public UnaryStatistic
{
public:
   SpectralMedian()
  : UnaryStatistic ("ftm", "robust estimate of spectral noise variance")
  {
  }

  double get (const vector<double>& data)
  {
    // divide by log(2) because spectral power has exponential distribution
    return robust_variance (data);
  }

  SpectralMedian* clone () const { return new SpectralMedian(*this); }
};

class MaxUpperHarmonic : public UnaryStatistic
{
public:
   MaxUpperHarmonic()
  : UnaryStatistic ("mh2", "maximum harmonic in upper half-spectrum")
  {
  }

  double get (const vector<double>& data)
  {
    vector<float> fps;
    power_spectral_density (data, fps);
    vector<double> upper_half (fps.begin() + fps.size()/2, fps.end());
    return *std::max_element (upper_half.begin(), upper_half.end());
  }

  MaxUpperHarmonic* clone () const { return new MaxUpperHarmonic(*this); }
};

class SpectralPower : public UnaryStatistic
{
public:
   SpectralPower()
  : UnaryStatistic ("sm2", "total power in upper half-spectrum")
  {
  }

  double get (const vector<double>& data)
  {
    vector<float> fps;
    spectrum (data, fps);
    vector<double> upper_half (fps.begin() + fps.size()/2, fps.end());

    vector<double> mu (2);
    central_moments (upper_half, mu);
      
    return mu[1];
  }

  SpectralPower* clone () const { return new SpectralPower(*this); }
};

class SpectralSkew : public UnaryStatistic
{
public:
   SpectralSkew()
  : UnaryStatistic ("sm3", "skew of upper half-spectrum")
  {
  }

  double get (const vector<double>& data)
  {
    vector<float> fps;
    spectrum (data, fps);
    vector<double> upper_half (fps.begin() + fps.size()/2, fps.end());

    vector<double> mu (3);
    central_moments (upper_half, mu);
      
    double sigma = sqrt( mu[1] );
    return mu[2] / pow(sigma,3.0);
  }

  SpectralSkew* clone () const { return new SpectralSkew(*this); }
};

class SpectralKurtosis : public UnaryStatistic
{
public:
   SpectralKurtosis()
  : UnaryStatistic ("sm4", "kurtosis of upper half-spectrum")
  {
  }

  double get (const vector<double>& data)
  {
    vector<float> fps;
    spectrum (data, fps);
    vector<double> upper_half (fps.begin() + fps.size()/2, fps.end());

    vector<double> mu (4);
    central_moments (upper_half, mu);
      
    return mu[3] / (mu[1]*mu[1]);
  }

  SpectralKurtosis* clone () const { return new SpectralKurtosis(*this); }
};

class SumHarmonicOutlier : public UnaryStatistic
{
  float threshold;
  
public:
   SumHarmonicOutlier()
  : UnaryStatistic ("sho", "sum of harmonic outlier power")
  {
    add_alias ("harmout");
    threshold = 2.0;        // standard deviations
  }

  double get (const vector<double>& data)
  {
    vector<float> fps;
    double log_mean = log( robust_variance(data, &fps) );

    for (unsigned i=0; i < fps.size(); i++)
      fps[i] = log(fps[i]);

    double log_var = log(threshold*threshold);
 
    double outlier_sum = 0.0;
    for (unsigned i=0; i+2 < fps.size(); i++)
    {
      fps[i] = fps[i+1] - std::max(log_mean, ( fps[i] + fps[i+2] )/2.0);
      if (fps[i] > log_var)
	outlier_sum += exp(fps[i]);
    }

    return outlier_sum;
  }

  SumHarmonicOutlier* clone () const { return new SumHarmonicOutlier(*this); }
};

void detrend (vector<double>& data)
{
  const unsigned ndat = data.size();
  
  vector<double> local_mean (ndat);

  for (unsigned i=0; i<ndat; i++)
    local_mean[i] = (data[(i+1)%ndat] + data[(i+(ndat-1))%ndat]) / 2.0;

  for (unsigned i=0; i<ndat; i++)
    data[i] -= local_mean[i];
}

class SumDetrendedOutlier : public UnaryStatistic
{
  float threshold;
  
public:
   SumDetrendedOutlier()
  : UnaryStatistic ("sdo", "sum of detrended outlier power")
  {
    threshold = 2.0;        // standard deviations
  }

  double get (const vector<double>& data)
  {
    vector<double> detrended (data.begin(), data.end());
    detrend (detrended);

    // 0.5 = extra variance due to detrend
    double var = 1.5 * robust_variance (data);

    double outlier = threshold*threshold*var;
    double outlier_sum = 0.0;
    for (unsigned i=0; i < data.size(); i++)
    {
      if ( detrended[i]*detrended[i] > outlier)
	outlier_sum += detrended[i]*detrended[i];
    }

    return outlier_sum;
  }

  SumDetrendedOutlier* clone () const { return new SumDetrendedOutlier(*this); }
};



class NyquistHarmonic : public UnaryStatistic
{
public:
  NyquistHarmonic ()
  : UnaryStatistic ("ftN", "last harmonic of values")
  { 
    add_alias ("fftN");
    add_alias ("Nyquist");
  }

  double get (const vector<double>& data)
  {
    vector<float> fourier;
    power_spectral_density (data, fourier);
    return fourier.back();
  }

  NyquistHarmonic* clone () const { return new NyquistHarmonic(*this); }
};

class SpectralEntropy : public UnaryStatistic
{
public:
   SpectralEntropy()
  : UnaryStatistic ("fte", "spectral entropy")
  {
    add_alias ("se");
    add_alias ("entropy");
  }

  double get (const vector<double>& data)
  {
    vector<float> fps;
    power_spectral_density (data, fps);

    unsigned istart = 1; // skip DC
    
    double sum = std::accumulate (fps.begin()+istart, fps.end(), 0.0);

    double entropy = 0;
    for (unsigned i=istart; i < fps.size(); i++)
      {
	double p = fps[i] / sum;
	entropy -= p * log(p);
      }
    
    return entropy;
  }

  SpectralEntropy* clone () const { return new SpectralEntropy(*this); }
};

} // namespace UnaryStatistics

static unsigned instance_count = 0;

UnaryStatistic::UnaryStatistic (const string& name, const string& description)
{
  instance_count ++;

  set_identity (name);
  set_description (description);
}

#include "identifiable_factory.h"

static std::vector< UnaryStatistic* >* instances = NULL;

using namespace UnaryStatistics;

void UnaryStatistic::build ()
{
  // ThreadContext::Lock lock (context);

  if (instances != NULL)
    return;

  // cerr << "UnaryStatistic::build" << endl;
 
  instances = new std::vector< UnaryStatistic* >;
 
  unsigned start_count = instance_count;
 
  instances->push_back( new Minimum );
  instances->push_back( new Maximum );
  instances->push_back( new Range );
  instances->push_back( new Sum );
  instances->push_back( new Mean );
  instances->push_back( new StandardDeviation );
  instances->push_back( new Skewness );
  instances->push_back( new Kurtosis );
  instances->push_back( new DeviationCoefficient );
  instances->push_back( new Median );
  instances->push_back( new MedianAbsoluteDifference );
  instances->push_back( new InterQuartileRange );
  instances->push_back( new QuartileDispersion );
  instances->push_back( new QuartileSkewness );
  instances->push_back( new OctileKurtosis );
  instances->push_back( new FirstHarmonic );
  instances->push_back( new NyquistHarmonic );
  instances->push_back( new MaxUpperHarmonic );
  instances->push_back( new SpectralPower );
  instances->push_back( new SpectralSkew );
  instances->push_back( new SpectralKurtosis );
  instances->push_back( new SpectralMedian );
  instances->push_back( new SumHarmonicOutlier );
  instances->push_back( new SumDetrendedOutlier );
  instances->push_back( new SpectralEntropy );


  // cerr << "UnaryStatistic::build instances=" << instances << " count=" << instance_count << " start=" << start_count << " size=" << instances->size() << endl; 

  assert (instances->size() == instance_count - start_count);

}


const std::vector< UnaryStatistic* >& UnaryStatistic::children ()
{
  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return *instances;
}

UnaryStatistic*
UnaryStatistic::factory (const std::string& name)
{
  return identifiable_factory<UnaryStatistic> (children(), name);
}

