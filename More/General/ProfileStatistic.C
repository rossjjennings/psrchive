/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatistic.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Fourier.h"

#include <algorithm>
#include <cassert>

using namespace std;
using namespace Pulsar;

class Maximum : public ProfileStatistic
{     
public:
  Maximum ()
  : ProfileStatistic ("max", "maximum profile amplitude")
  {
    add_alias ("maximum");
  }

  double get (const Profile* profile)
  { return profile->max(); }

  Maximum* clone () const { return new Maximum(*this); }

};

class Minimum : public ProfileStatistic
{
public:
  Minimum ()
  : ProfileStatistic ("min", "minimum profile amplitude")
  {
    add_alias ("minimum");
  }

  double get (const Profile* profile)
  { return profile->min(); }

  Minimum* clone () const { return new Minimum(*this); }

};

class PeakToPeak : public ProfileStatistic
{
public:
  PeakToPeak ()
  : ProfileStatistic ("range", "range of profile amplitudes")
  {
    // cerr << "PeakToPeak ctor this=" << this << endl;
    add_alias ("peak-to-peak");
  }

  // ~PeakToPeak () { cerr << "PeakToPeak dtor this=" << this << endl; }

  double get (const Profile* profile)
  { return profile->max() - profile->min(); }

  PeakToPeak* clone () const { return new PeakToPeak(*this); }
};

class Mean : public ProfileStatistic
{
public:
  Mean () : ProfileStatistic ("mean", "mean profile amplitude")
  {
    add_alias ("average");
    add_alias ("avg");
  }

  double get (const Profile* profile)
  { return profile->sum() / profile->get_nbin(); }

  Mean* clone () const { return new Mean(*this); }
};

class StandardDeviation : public ProfileStatistic
{
public:
  StandardDeviation () 
  : ProfileStatistic ("rms", "standard deviation of profile amplitudes")
  {
    add_alias ("stddev");
    add_alias ("sigma");
  }

  double get (const Profile* profile)
  { return sqrt( profile->sumsq() / profile->get_nbin() ); }

  StandardDeviation* clone () const { return new StandardDeviation(*this); }
};

// https://en.wikipedia.org/wiki/Coefficient_of_variation
class DeviationCoefficient : public ProfileStatistic
{     
public:
  DeviationCoefficient ()
  : ProfileStatistic ("dev", "coefficient of deviation of profile amplitudes") 
  {
    add_alias ("modulation");
    add_alias ("beta");
    add_alias ("cv");    // coefficient of variation
    add_alias ("rsd");   // relative standard deviation
  }

  double get (const Profile* profile)
  { return sqrt( profile->sumsq() * profile->get_nbin() ) / profile->sum(); }

  DeviationCoefficient* clone () const { return new DeviationCoefficient(*this); }
};

double median (vector<float>& amps)
{
  unsigned mid = amps.size() / 2;
  std::nth_element( amps.begin(), amps.begin()+mid, amps.end() );
  return amps[mid];
}

double median (const Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  vector<float> amps (profile->get_amps(), profile->get_amps() + nbin);
  return median (amps);
}

class Median : public ProfileStatistic
{    
public:
  Median ()
  : ProfileStatistic ("med", "median of profile amplitudes")
  {
    add_alias ("median");
  }

  double get (const Profile* profile)
  { return median (profile); }

  Median* clone () const { return new Median(*this); }
};

double madm (const Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  vector<float> amps (profile->get_amps(), profile->get_amps() + nbin);
  float med = median (amps);
  for (unsigned i=0; i<nbin; i++)
    amps[i] = fabs( amps[i] - med );
  return median (amps);
}

class MedianAbsoluteDifference : public ProfileStatistic
{
public:
  MedianAbsoluteDifference ()
  : ProfileStatistic ("mad", "median absolute difference from median")
  {
    add_alias ("mdm");
    add_alias ("madm");
  }

  double get (const Profile* profile)
  { return madm (profile); }

  MedianAbsoluteDifference* clone () const 
  { return new MedianAbsoluteDifference(*this); }

};

void Q1_Q3 (vector<float>& amps, float& Q1, float& Q3)
{
  std::sort( amps.begin(), amps.end() );
  unsigned nbin = amps.size();

  unsigned iQ1 = nbin / 4;
  unsigned iQ3 = (3 * nbin) / 4;

  Q1 = amps[iQ1];
  Q3 = amps[iQ3];
}

double iqr (vector<float>& amps)
{
  float Q1=0, Q3=0;
  Q1_Q3 (amps, Q1, Q3);
  return Q3 - Q1;
}

double iqr (const Profile* profile)
{ 
  unsigned nbin = profile->get_nbin();
  vector<float> amps (profile->get_amps(), profile->get_amps() + nbin);
  return iqr (amps);
}

class InterQuartileRange : public ProfileStatistic
{
public:
  InterQuartileRange ()
  : ProfileStatistic ("iqr", "inter-quartile range of profile amps")
  {
  }

  double get (const Profile* profile)
  { return iqr (profile); }

  InterQuartileRange* clone () const 
  { return new InterQuartileRange(*this); }

};

double qcd (vector<float>& amps)
{ 
  float Q1=0, Q3=0;
  Q1_Q3 (amps, Q1, Q3);
  return (Q3 - Q1) / (Q3 + Q1); 
}

double qcd (const Profile* profile)
{
  unsigned nbin = profile->get_nbin();
  vector<float> amps (profile->get_amps(), profile->get_amps() + nbin);
  return qcd (amps);
}

// https://en.wikipedia.org/wiki/Quartile_coefficient_of_dispersion
class QuartileDispersion : public ProfileStatistic
{
public:
  QuartileDispersion ()
  : ProfileStatistic ("qcd", "quartile coefficient of dispersion")
  {
  }

  double get (const Profile* profile)
  { return qcd (profile); }

  QuartileDispersion* clone () const
  { return new QuartileDispersion(*this); }
};

class RelativeIQR : public ProfileStatistic
{
public:
  RelativeIQR ()
  : ProfileStatistic ("iqr/med", "inter-quartile range divided by median")
  {
    add_alias ("reliq");
  }

  double get (const Profile* profile)
  { return iqr (profile) / median (profile); }

  RelativeIQR* clone () const
  { return new RelativeIQR(*this); }
};

class RobustRange : public ProfileStatistic
{
public:
  RobustRange ()
  : ProfileStatistic ("robust_range", "range normalized by median")
  {
    add_alias ("range/med");
  }

  double get (const Profile* profile)
  { return (profile->max() - profile->min()) / median(profile); }

  RobustRange* clone () const { return new RobustRange(*this); }
};

class FirstHarmonic : public ProfileStatistic
{
public:
  FirstHarmonic ()
  : ProfileStatistic ("ft1", "first harmonic of profile amplitudes")
  {
    add_alias ("fft1");
    add_alias ("fundamental");
  }

  double get (const Profile* profile)
  { 
    Reference::To<Profile> fft = fourier_transform (profile);
    detect (fft);
    return fft->get_amps()[1];
  }

  FirstHarmonic* clone () const { return new FirstHarmonic(*this); }
};

class MaxHarmonicRatio : public ProfileStatistic
{
public:
   MaxHarmonicRatio()
  : ProfileStatistic ("mh1", "maximum ratio with first harmonic")
  {
    add_alias ("mhr1");
    add_alias ("harmonic");
  }

  double get (const Profile* profile)
  {
    Reference::To<Profile> fft = fourier_transform (profile);
    detect (fft);
    // start searching for the max at the second harmonic
    double max = fft->max(2);
    return max / fft->get_amps()[1];
  }

  MaxHarmonicRatio* clone () const { return new MaxHarmonicRatio(*this); }
};

class NyquistHarmonic : public ProfileStatistic
{
public:
  NyquistHarmonic ()
  : ProfileStatistic ("ftN", "last harmonic of profile amplitudes")
  { 
    add_alias ("fftN");
    add_alias ("Nyquist");
  }

  double get (const Profile* profile)
  { 
    Reference::To<Profile> fft = fourier_transform (profile);
    detect (fft);
    return fft->get_amps()[fft->get_nbin()-1];
  }

  NyquistHarmonic* clone () const { return new NyquistHarmonic(*this); }
};

static unsigned instance_count = 0;

Pulsar::ProfileStatistic::ProfileStatistic (const string& name, 
                                            const string& description)
{
  instance_count ++;

  set_identity (name);
  set_description (description);
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
 
  instances->push_back( new Minimum );
  instances->push_back( new Maximum );
  instances->push_back( new PeakToPeak );
  instances->push_back( new Mean );
  instances->push_back( new StandardDeviation );
  instances->push_back( new DeviationCoefficient );
  instances->push_back( new Median );
  instances->push_back( new MedianAbsoluteDifference );
  instances->push_back( new InterQuartileRange );
  instances->push_back( new QuartileDispersion );
  instances->push_back( new RelativeIQR );
  instances->push_back( new RobustRange );
  instances->push_back( new FirstHarmonic );
  instances->push_back( new NyquistHarmonic );
  instances->push_back( new MaxHarmonicRatio );

  assert (instances->size() == instance_count - start_count);

  // cerr << "Pulsar::ProfileStatistic::build instances=" << instances << endl;
}


Pulsar::ProfileStatistic*
Pulsar::ProfileStatistic::factory (const std::string& name)
{
  // cerr << "Pulsar::ProfileStatistic::factory instances=" << instances << endl;

  if (instances == NULL)
    build ();

  assert (instances != NULL);

  // cerr << "ProfileStatistic::factory instances=" << instances << endl;

  ProfileStatistic* stat = 0;
  stat = identifiable_factory<ProfileStatistic> (*instances, name);

  // cerr << "Pulsar::ProfileStatistic::factory return=" << stat << endl;
  return stat;
}

