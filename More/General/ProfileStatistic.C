/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ProfileStatistic.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Fourier.h"

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
};

class Range : public ProfileStatistic
{
public:
  Range ()
  : ProfileStatistic ("range", "range of profile amplitudes")
  {
    add_alias ("peak-to-peak");
  }

  double get (const Profile* profile)
  { return profile->max() - profile->min(); }
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
};

class DeviationCoefficient : public ProfileStatistic
{     
public:
  DeviationCoefficient ()
  : ProfileStatistic ("dev", "coefficient of deviation of profile amplitudes") 
  {
    add_alias ("modulation");
    add_alias ("beta");
  }
  double get (const Profile* profile)
  { return sqrt( profile->sumsq() * profile->get_nbin() ) / profile->sum(); }
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
};

Pulsar::ProfileStatistic::ProfileStatistic (const string& name, 
                                            const string& description)
{
  set_identity (name);
  set_description (description);
}

#include "identifiable_factory.h"

Pulsar::ProfileStatistic*
Pulsar::ProfileStatistic::factory (const std::string& name)
{
  std::vector< Reference::To<Agent> > instances;
  instances.push_back( new Advocate<Minimum> );
  instances.push_back( new Advocate<Maximum> );
  instances.push_back( new Advocate<Range> );
  instances.push_back( new Advocate<Mean> );
  instances.push_back( new Advocate<StandardDeviation> );
  instances.push_back( new Advocate<DeviationCoefficient> );
  instances.push_back( new Advocate<Median> );
  instances.push_back( new Advocate<MedianAbsoluteDifference> );
  instances.push_back( new Advocate<FirstHarmonic> );
  instances.push_back( new Advocate<NyquistHarmonic> );

  return identifiable_factory<ProfileStatistic> (instances, name);
}

