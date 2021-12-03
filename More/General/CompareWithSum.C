/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CompareWithSum.h"
#include "Pulsar/Profile.h"

#include "BinaryStatistic.h"
#include "UnaryStatistic.h"

// #define _DEBUG 1
#include "debug.h"

using namespace Pulsar;
using namespace std;

void CompareWithSum::setup (unsigned start_primary, unsigned nprimary)
{
  CompareWith::setup (start_primary, nprimary);

  if (!mean)
    compute_mean (start_primary, nprimary);
}

void CompareWithSum::compute (unsigned iprimary, ndArray<2,double>& result)
{
  if (!mean)
  {
    for (unsigned icompare=0; icompare < ncompare; icompare++)
      set (result, iprimary, icompare, 0.0);	

    return;
  }
	
  vector<double> sumdata (mean->get_amps(),
			  mean->get_amps() + mean->get_nbin());

  double rms = sqrt( robust_variance (sumdata) );
  for (double& element : sumdata)
    element /= rms;
	      
  for (unsigned icompare=0; icompare < ncompare; icompare++)
  {
    (data->*compare) (icompare);
    
    Reference::To<const Profile> iprof = data->get_Profile ();

    if (iprof->get_weight() == 0.0)
    {
      set (result, iprimary, icompare, 0.0);	
      continue;
    }
      
    vector<double> idata (iprof->get_amps(),
			  iprof->get_amps() + iprof->get_nbin());

    rms = sqrt( robust_variance (idata) );
    for (double& element : idata)
      element /= rms;
	      
#ifdef _DEBUG
    cerr << "CompareWithSum::compute calling BinaryStatistic::get" << endl;
#endif
	
    double val = statistic->get (idata, sumdata);

#if _DEBUG
    cerr << "CompareWithSum::compute iprim=" << iprimary
	 << " icomp=" << icompare << " val=" << val << endl;
#endif
    
    set (result, iprimary, icompare, val);
  }
}
