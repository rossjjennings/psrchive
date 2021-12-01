/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CompareWithEachOther.h"
#include "Pulsar/Profile.h"

#include "BinaryStatistic.h"
#include "UnaryStatistic.h"

using namespace Pulsar;
using namespace std;

CompareWithEachOther::CompareWithEachOther ()
{
  summary = UnaryStatistic::factory ("median");
}

void CompareWithEachOther::set_summary (UnaryStatistic* stat)
{
  summary = stat;
}

void CompareWithEachOther::compute (unsigned iprimary, ndArray<2,double>& result)
{
  // resize the temporary ndArray
  temp * ncompare * ncompare;

#ifdef _DEBUG
  cerr << "CompareWithEachOther::build nsubint=" << nsubint
       << " nchan=" << nchan << endl;
#endif

  rms.resize (ncompare, 0.0);
  unsigned irms=0;
    
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
    
    if (irms < icompare)
    {
      irms = icompare;
      rms[irms] = sqrt( robust_variance (idata) );
    }
    
    for (double& element : idata)
      element /= rms[icompare];

    for (unsigned jcompare=icompare+1; jcompare < ncompare; jcompare++)
    {
      (data->*compare) (jcompare);

      Reference::To<const Profile> jprof = data->get_Profile ();
      
      if (jprof->get_weight() == 0.0)
      {
	temp[icompare][jcompare] = temp[jcompare][icompare] = 0.0;
	continue;
      }
      
      vector<double> jdata (jprof->get_amps(),
			    jprof->get_amps() + jprof->get_nbin());
      
      if (irms < jcompare)
      {
	irms = jcompare;
	rms[irms] = sqrt( robust_variance (jdata) );
      }
      
      for (double& element : jdata)
	element /= rms[jcompare];
      
#ifdef _DEBUG
      cerr << "calling BinaryStatistic::get" << endl;
#endif
	
      double val = statistic->get (idata, jdata);
      
      temp[icompare][jcompare] = temp[jcompare][icompare] = val;
    }
    
    std::vector<double> data (ncompare-1);
    unsigned ndat = 0;
    
    for (unsigned jcompare=0; jcompare < ncompare; jcompare++)
    {
      if (jcompare == icompare)
	continue;
      
      if (temp[icompare][jcompare] == 0.0)
	continue;
      
      data[ndat] = temp[icompare][jcompare];
      ndat ++;
    }
    
    if (ndat == 0)
      set (result, iprimary, icompare, 0.0);	
    else
    {
      data.resize (ndat);
      set (result, iprimary, icompare, summary->get(data));
    }
  }
}
