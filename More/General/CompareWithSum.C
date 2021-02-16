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

void CompareWithSum::compute (ndArray<2,double>& result)
{
  check ("CompareWithSum::compute");
  
  for (unsigned iprimary=0; iprimary < nprimary; iprimary++)
  {
    (data->*primary) (iprimary);

    Reference::To<const Profile> sum;
    
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
	sum->sum (iprof);
    }

    if (!sum)
      continue;
    
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
      cerr << "calling BinaryStatistic::get" << endl;
#endif
	
      double val = statistic->get (idata, sumdata);
      set (result, iprimary, icompare, val);
    }
  }
}
