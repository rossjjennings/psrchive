/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CompareWith.h"
#include "Pulsar/TimeDomainCovariance.h"
#include "Pulsar/Profile.h"

#include "GeneralizedChiSquared.h"
#include "UnaryStatistic.h"

using namespace Pulsar;
using namespace std;

CompareWith::CompareWith ()
{
  data = 0;
  statistic = 0;
  primary = 0;
  compare = 0;
  transpose = false;
}

void CompareWith::set_statistic (BinaryStatistic* stat)
{
  statistic = stat;
}

void CompareWith::set_data (HasArchive* _data)
{
  data = _data;
}

void CompareWith::set_primary (unsigned n,
			       void (HasArchive::*func) (Index))
{
  nprimary = n;
  primary = func;
}

void CompareWith::set_compare (unsigned n,
			       void (HasArchive::*func) (Index))
{
  ncompare = n;
  compare = func;
}

void CompareWith::set_transpose (bool f)
{
  transpose = f;
}

void CompareWith::set (ndArray<2,double>& result,
		       unsigned iprimary, unsigned icompare,
		       double value)
{
#if _DEBUG
  cerr << "CompareWith::set i=" << iprimary << " j=" << icompare
       << " val=" << value << endl;
#endif
  
  if (transpose)
    result[icompare][iprimary] = value;
  else
    result[iprimary][icompare] = value;
}

void CompareWith::check ()
{
  if (!data)
    throw Error (InvalidState, "CompareWith::check", "archive data not set");

  if (!statistic)
    throw Error (InvalidState, "CompareWith::check", "binary statistic not set");

  if (!primary)
    throw Error (InvalidState, "CompareWith::check", "primary index method not set");

  if (!compare)
    throw Error (InvalidState, "CompareWith::check", "compare index method not set");
}

void CompareWith::compute (ndArray<2,double>& result)
{
  check ();
  
  for (unsigned iprimary=0; iprimary < nprimary; iprimary++) try
  {
    (data->*primary) (iprimary);

    setup (iprimary);
    
    compute (iprimary, result);
  }
  catch (Error& error)
  {
    for (unsigned icompare=0; icompare < ncompare; icompare++)
      set (result, iprimary, icompare, 0.0);
    
    continue;
  }

}

using BinaryStatistics::GeneralizedChiSquared;

void CompareWith::setup (unsigned iprimary)
{
  GeneralizedChiSquared* gcs = dynamic_cast<GeneralizedChiSquared*> (statistic.get());
  if (!gcs)
    return;

  // cerr << "CompareWith::setup iprimary=" << iprimary << endl;

  if (!covar)
    covar = new TimeDomainCovariance;

  covar->reset ();

  double var = 0.0;
  double norm = 0.0;
  
  for (unsigned icompare=0; icompare < ncompare; icompare++)
  {
    (data->*compare) (icompare);

    Reference::To<const Profile> prof = data->get_Profile ();

    double weight = prof->get_weight();

    if (weight == 0.0)
      continue;

    vector<double> amps (prof->get_amps(),
			 prof->get_amps() + prof->get_nbin());

    var += weight * robust_variance (amps);
    norm += weight;
    
    covar->add_Profile (prof);
  }

  var /= norm;
  
  // cerr << "CompareWith::setup var=" << var << endl;

  covar->eigen ();

  // cerr << "CompareWith::setup eigen" << endl;

  const double* eval = covar->get_eigenvalues_pointer();
  const double* evec = covar->get_eigenvectors_pointer();
  
  unsigned nrank = covar->get_rank();
  unsigned eff_rank = 0;
  while (eff_rank < nrank && eval[eff_rank] > var)
    eff_rank ++;

  gcs->eigenvectors * eff_rank * nrank;
  gcs->eigenvalues * eff_rank;
  
  for (unsigned irank=0; irank < eff_rank; irank++)
  {
    for (unsigned jrank=0; jrank < nrank; jrank++)
      gcs->eigenvectors[irank][jrank] = evec[irank*nrank+jrank];

    /*
      divide by var because both CompareWithSum and CompareWithEachOther normalize
      each profile by the robust_variance
    */
    gcs->eigenvalues[irank] = eval[irank] / var;
  }

  // cerr << "CompareWith::setup done" << endl;
}
    
