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
  
  // worker function for variance, skewness, kurtosis, etc.
  void central_moments (vector<double> amps, vector<double>& mu)
  {
    if (mu.size() == 0)
      return;

    vector<double> prod (amps.size(), 1.0);

    for (unsigned i=0; i < mu.size(); i++)
      {
	double total = 0.0;
	
	for (unsigned j=0; j < amps.size(); j++)
	  {
	    prod[j] *= amps[j];
	    total += prod[j];
	  }
	
	mu[i] = total / amps.size();
	
	if (i == 0)
	  {
	    // subract the mean so that further moments are central
	    for (unsigned j=0; j < amps.size(); j++)
	      {
		prod[j] -= mu[0];
		amps[j] = prod[j];
	      }
	  }
      }
  }
  
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
  
  double median (vector<double> data)
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

    for (auto& element : data)
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
  : UnaryStatistic ("iqr", "inter-quartile range of amps")
  {
  }

  double get (const vector<double>& data)
  { return iqr (data); }

  InterQuartileRange* clone () const 
  { return new InterQuartileRange(*this); }

};

double qcd (const vector<double>& amps)
{ 
  double Q1=0, Q2=0, Q3=0;
  Q1_Q2_Q3 (amps, Q1, Q2, Q3);
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

double qcs (const vector<double>& amps)
{
  double Q1=0, Q2=0, Q3=0;
  Q1_Q2_Q3 (amps, Q1, Q2, Q3);
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
void Ntile (vector<double> amps, vector<double>& Ntiles)
{
  std::sort( amps.begin(), amps.end() );
  unsigned nbin = amps.size();
  unsigned N = Ntiles.size() + 1;

  for (unsigned i=0; i < Ntiles.size(); i++)
  {
    unsigned itile = (nbin * (i+1)) / N;
    Ntiles[i] = amps[itile];
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

void fluctuation_power_spectrum (const vector<double>& data, vector<float>& fps)
{
  vector<float> copy (data.begin(), data.end());
  fps.resize (data.size() + 2);
  
  FTransform::frc1d (data.size(), &fps[0], &copy[0]);

  const unsigned nbin = data.size() / 2;
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
    fluctuation_power_spectrum (data, fourier);
    return fourier[1];
  }

  FirstHarmonic* clone () const { return new FirstHarmonic(*this); }
};

class MaxHarmonicRatio : public UnaryStatistic
{
public:
   MaxHarmonicRatio()
  : UnaryStatistic ("mh1", "maximum ratio with first harmonic")
  {
    add_alias ("mhr1");
    add_alias ("harmonic");
  }

  double get (const vector<double>& data)
  {
    vector<float> fps;
    fluctuation_power_spectrum (data, fps);
    return *std::max_element (fps.begin()+20, fps.end()) / fps[1];
  }

  MaxHarmonicRatio* clone () const { return new MaxHarmonicRatio(*this); }
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
    fluctuation_power_spectrum (data, fps);

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
    fluctuation_power_spectrum (data, fourier);
    return fourier.back();
  }

  NyquistHarmonic* clone () const { return new NyquistHarmonic(*this); }
};

} // namespace UnaryStatistics

static unsigned instance_count = 0;

UnaryStatistic::UnaryStatistic (const string& name, 
					const string& description)
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
  instances->push_back( new MaxHarmonicRatio );
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

