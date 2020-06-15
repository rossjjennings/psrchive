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

  Range* clone () const { return new Range(*this); }
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

  MedianAbsoluteDifference* clone () const { return new MedianAbsoluteDifference(*this); }

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

Pulsar::ProfileStatistic::ProfileStatistic (const string& name, 
                                            const string& description)
{
  set_identity (name);
  set_description (description);
}

#include "identifiable_factory.h"

#if HAVE_PTHREAD
static ThreadContext* context = new ThreadContext;
#else
static ThreadContext* context = 0;
#endif

static std::vector< Pulsar::ProfileStatistic* >* instances = NULL;

void Pulsar::ProfileStatistic::build ()
{
  ThreadContext::Lock lock (context);

  if (instances != NULL)
    return;

  cerr << "Pulsar::ProfileStatistic::build" << endl;
 
  instances = new std::vector< ProfileStatistic* >;
  
  instances->push_back( new Minimum );
  instances->push_back( new Maximum );
  instances->push_back( new Range );
  instances->push_back( new Mean );
  instances->push_back( new StandardDeviation );
  instances->push_back( new DeviationCoefficient );
  instances->push_back( new Median );
  instances->push_back( new MedianAbsoluteDifference );
  instances->push_back( new FirstHarmonic );
  instances->push_back( new NyquistHarmonic );

  cerr << "Pulsar::ProfileStatistic::build instances=" << instances << endl;
}


Pulsar::ProfileStatistic*
Pulsar::ProfileStatistic::factory (const std::string& name)
{
  cerr << "Pulsar::ProfileStatistic::factory instances=" << instances << endl;

  if (instances == NULL)
    build ();

  assert (instances != NULL);

  return identifiable_factory<ProfileStatistic> (*instances, name);
}

