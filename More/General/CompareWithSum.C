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

using namespace Pulsar;
using namespace std;

void CompareWithSum::compute (unsigned iprimary, ndArray<2,double>& result)
{
  Reference::To<Profile> sum = 0;
    
  for (unsigned icompare=0; icompare < ncompare; icompare++)
  {
    (data->*compare) (icompare);

    Reference::To<const Profile> iprof = data->get_Profile ();

    if (iprof->get_weight() == 0.0)
    {
      set (result, iprimary, icompare, 0.0);	
      continue;
    }
      
    if (!sum)
      sum = iprof->clone();
    else
      sum->average (iprof);
  }

  if (!sum)
    return;
    
  vector<double> sumdata (sum->get_amps(),
			  sum->get_amps() + sum->get_nbin());

  double rms = sqrt( robust_variance (sumdata) );
  for (double& element : sumdata)
    element /= rms;
	      
  for (unsigned icompare=0; icompare < ncompare; icompare++)
  {
    (data->*compare) (icompare);
    
    Reference::To<const Profile> iprof = data->get_Profile ();

    if (iprof->get_weight() == 0.0)
      continue;

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
