/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/CompareWith.h"
#include "Pulsar/TimeDomainCovariance.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"

#include "GaussianMixtureProbabilityDensity.h"
#include "GeneralizedChiSquared.h"
#include "ChiSquared.h"
#include "UnaryStatistic.h"

// #define _DEBUG 1
#include "debug.h"

#if HAVE_ARMADILLO
#include <armadillo>
using namespace arma;
#endif

using namespace Pulsar;
using namespace std;

CompareWith::CompareWith ()
{
  data = 0;
  statistic = 0;
  primary = 0;
  compare = 0;
  transpose = false;
  compare_all = false;

  model_residual = true;
  use_null_space = false;
  
  is_setup = false;
  setup_completed = false;

  bscrunch_factor.disable_scrunch();

  fptr = 0;
}

void CompareWith::set_statistic (BinaryStatistic* stat)
{
  statistic = stat;
}

void CompareWith::set_data (HasArchive* _data)
{
  data = _data;
}

void CompareWith::set_setup_data (const Archive* _data)
{
  is_setup = false;

  if (_data == NULL)
    return;

  if (!compare_all)
    return;
  
  Reference::To<const Archive> target = data->get_Archive();
  data->set_Archive (_data);

  setup (0, nprimary);

  is_setup = setup_completed;
  data->set_Archive (target);
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
  if (transpose)
    result[icompare][iprimary] = value;
  else
    result[iprimary][icompare] = value;
}

void CompareWith::check ()
{
  DEBUG("CompareWith::check");

  if (!data)
    throw Error (InvalidState, "CompareWith::check", "archive data not set");

  if (!statistic)
    throw Error (InvalidState, "CompareWith::check", "binary statistic not set");

  if (!primary)
    throw Error (InvalidState, "CompareWith::check", "primary index method not set");

  if (!compare)
    throw Error (InvalidState, "CompareWith::check", "compare index method not set");

  DEBUG("CompareWith::check fptr=" << (void*) fptr);

  statistic->set_file (fptr);

  DEBUG("CompareWith::check done");
}

void CompareWith::compute (ndArray<2,double>& result)
{
  check ();

  DEBUG("CompareWith::compute nprimary=" << nprimary << " mask.size=" << compute_mask.size() << " all=" << compare_all);

  if (compare_all && !is_setup)
    setup (0, nprimary);

  DEBUG("CompareWith::compute setup done");
  
  for (unsigned iprimary=0; iprimary < nprimary; iprimary++) try
  {
    if (compute_mask.size() == nprimary && !compute_mask[iprimary])
      continue;
    
    (data->*primary) (iprimary);

    if (!compare_all && !is_setup)
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

using namespace BinaryStatistics;

void CompareWith::get_amps (vector<double>& amps, const Profile* profile)
{
  unsigned nbin = profile->get_nbin ();
  
  amps = vector<double> ( profile->get_amps(),
			  profile->get_amps() + nbin );

  double variance = robust_variance (amps);

  if (bscrunch_factor.scrunch_enabled())
  {
    if (!temp)
      temp = profile->clone();

    temp->set_amps (amps);
    Pulsar::bscrunch (temp.get(), bscrunch_factor);

    amps = vector<double> ( temp->get_amps(),
			    temp->get_amps() + temp->get_nbin() );

    variance /= bscrunch_factor.get_nscrunch (nbin);
  }

  double rms = sqrt( variance );
  for (double& element : amps)
    element /= rms;
}

void CompareWith::get_residual (vector<double>& amps,
				const vector<double>& mamps)
{
  chi.set_outlier_threshold (0.0);

  double chisq = chi.get (amps, mamps);

  DEBUG( "CompareWith::get_residual chisq=" << chisq );

#if _DEBUG
	
  static bool once = true;
  if (once)
  {
    vector<double> resid = chi.get_residual();
    FILE* fptr = fopen ("resid.dat", "w");
    for (unsigned i=0; i<amps.size(); i++)
      fprintf (fptr, "%g %g %g\n", mamps[i], amps[i], resid[i]);
    fclose (fptr);
    once = false;
  }
#endif
	  
  amps = chi.get_residual();
}


void CompareWith::setup (unsigned start_primary, unsigned nprimary)
{
  mean = 0;

  setup_completed = false;

  bool needs_setup = false;
  
  GeneralizedChiSquared* gcs = 0;
  gcs = dynamic_cast<GeneralizedChiSquared*> (statistic.get());
  if (gcs)
    needs_setup = true;

#if HAVE_ARMADILLO

  GaussianMixtureProbabilityDensity* gmpd = 0;

  if (!gcs)
  {
    gmpd = dynamic_cast<GaussianMixtureProbabilityDensity*> (statistic.get());
    if (gmpd)
    {
      needs_setup = true;

      if (gmpd->gcs)
	gcs = gmpd->gcs;
      else
	gmpd->gcs = gcs = new GeneralizedChiSquared;
    }
  }
  
#endif
  
  if (!needs_setup)
    return;

  DEBUG( "CompareWith::setup start_primary=" << start_primary << " nprimary=" << nprimary);

  if (!covar)
    covar = new TimeDomainCovariance;

  covar->reset ();

  vector<double> mamps;
  
  if (model_residual)
  {
    compute_mean (start_primary, nprimary);
    
    if (!mean)
      throw Error (InvalidState, "CompareWith::setup", "no good data");

    get_amps (mamps, mean);
  }
  
  double var = 0.0;
  double norm = 0.0;

  unsigned nprofile = 0;
  
  for (unsigned iprimary=start_primary; iprimary < start_primary+nprimary; iprimary++)
  {
    (data->*primary) (iprimary);
    
    for (unsigned icompare=0; icompare < ncompare; icompare++)
    {
      (data->*compare) (icompare);

      Reference::To<const Profile> prof = data->get_Profile ();

      double weight = prof->get_weight();
      
      if (weight == 0.0)
	continue;

      vector<double> amps (prof->get_amps(),
			   prof->get_amps() + prof->get_nbin());

      if (model_residual)
      {
	get_amps (amps, prof);
	get_residual (amps, mamps);

	if (!temp)
	  temp = prof->clone();

	temp->set_amps (amps);
	prof = temp;
      }

      var += weight * robust_variance (amps);
      norm += weight;
      
      covar->add_Profile (prof);

      nprofile ++;
    }
  }
  
  if (norm == 0)
    throw Error (InvalidState, "CompareWith::setup", "no good data");
  
  var /= norm;
  
  DEBUG( "CompareWith::setup var=" << var );

  covar->eigen ();

  DEBUG( "CompareWith::setup eigen" );

  const double* eval = covar->get_eigenvalues_pointer();
  const double* evec = covar->get_eigenvectors_pointer();
  
  unsigned rank = std::min( covar->get_count(), covar->get_rank() );

  DEBUG("CompareWith::setup count=" << covar->get_count() << " dim=" << covar->get_rank() << " rank=" << rank);
  
  unsigned eff_rank = 0;
  unsigned offset = 0;

  if (use_null_space)
  {
    eff_rank = 2;
    offset = rank-2;

    cerr << "using null space" << endl;
  }
  else
  {
    while (eff_rank < rank && eval[eff_rank] > var)
      eff_rank ++;

    if (eff_rank > 0 && eff_rank+1 >= rank)
    {
      eff_rank --;
    
      DEBUG("CompareWith::setup full rank lambda/var=" << eval[eff_rank]/var);
    }
  
    DEBUG("CompareWith::setup effective rank=" << eff_rank);
  }
  
  unsigned nbin = covar->get_rank();

  if (gcs)
  {
    gcs->eigenvectors * eff_rank * nbin;
    gcs->eigenvalues * eff_rank;
  
    for (unsigned irank=0; irank < eff_rank; irank++)
    {
      for (unsigned ibin=0; ibin < nbin; ibin++)
	gcs->eigenvectors[irank][ibin] = evec[(irank+offset)*nbin+ibin];

      /*
	divide by var because both CompareWithSum and
	CompareWithEachOther normalize each profile by the square root
	of the robust_variance
      */
      gcs->eigenvalues[irank] = eval[irank+offset] / var;
    }
  }
  
  setup_completed = true;

  DEBUG( "CompareWith::setup eigen analysis completed" );
    
#if HAVE_ARMADILLO

  if (!gmpd)
    return;
  
  arma::mat data (eff_rank, nprofile, fill::zeros);

  // fill data matrix
  
  if (gmpd->model == NULL)
    gmpd->model = new arma::gmm_diag;
  
  arma::gmm_diag* model = gmpd->model;
  
  bool status = model->learn(data, 2, maha_dist, random_subset, 10, 5, 1e-10, true);
  if(status == false)  { cout << "learning failed" << endl; }
  model->means.print("means:");
  double overall_likelihood = model->avg_log_p(data);

  DEBUG( "CompareWith::setup Gaussian mixture analysis completed" );

#endif

}
    
void CompareWith::compute_mean (unsigned start_primary, unsigned nprimary)
{
  mean = 0;

  // DEBUG( "CompareWith::compute_mean start=" << start_primary << " n=" << nprimary);

  for (unsigned iprimary=start_primary; iprimary < start_primary+nprimary; iprimary++)
  {
    (data->*primary) (iprimary);
    
    for (unsigned icompare=0; icompare < ncompare; icompare++)
    {
      (data->*compare) (icompare);

      Reference::To<const Profile> iprof = data->get_Profile ();

      if (iprof->get_weight() == 0.0)
	continue;

      if (!mean)
	mean = iprof->clone();
      else
	mean->average (iprof);
    }
  }
}
