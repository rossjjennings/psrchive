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

#include "GeneralizedChiSquared.h"
#include "ChiSquared.h"
#include "UnaryStatistic.h"

#include <algorithm>
#include <cassert>
#include <fstream>

// #define _DEBUG 1
#include "debug.h"

#if HAVE_ARMADILLO
#include "GaussianMixtureProbabilityDensity.h"
#include <armadillo>
#endif

using namespace Pulsar;
using namespace std;

#if HAVE_ARMADILLO

static void initialize_armadillo_random_number_generator ()
{
  static bool done = false;

  if (done)
    return;

  arma::arma_rng::set_seed (13);
  done = true;
}

#endif

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

#if HAVE_ARMADILLO

void brute_force_search_for_best_gmm (arma::gmm_diag* model,
				      const arma::mat& data,
				      unsigned ngaus) try
{
  unsigned ntrial = 100;
    
  arma::rowvec best_hefts;
  arma::mat best_means;
  arma::mat best_dcovs;

  double max_distnorm = 0;
  double best_dist = 0;
  double best_var = 0;

  unsigned km_iter = 10;
  unsigned em_iter = 50;
  bool verbose = false;

  double var_floor = 1e-10;
  
  for (unsigned itrial=0; itrial < ntrial; itrial++)
  {
    bool status = model->learn(data, ngaus, arma::maha_dist,
			       arma::static_spread,
			       km_iter, em_iter, var_floor, verbose);

    if (status == false)
      throw Error (FailedCall, "CompareWith::setup",
		   "arma::gmm_diag::learn failed");

    arma::rowvec hefts = model->hefts;
    arma::mat means = model->means;
    arma::mat dcovs = model->dcovs;

    unsigned ndims = model->n_dims();
  
    double totvar = 0;
    double totdist = 0;
    double distnorm = 0;
    
    unsigned count = 0;
    
    for (unsigned igaus=0; igaus < ngaus; igaus++)
    {
      bool used = false;
      
      for (unsigned idim=0; idim < ndims; idim++)
	if (dcovs(idim, igaus) != var_floor)
	{
	  used = true;
	  break;
	}

      if (!used)
	continue;
      
      for (unsigned idim=0; idim < ndims; idim++)
      {
	totvar += hefts[igaus] * dcovs(idim, igaus);
	double distsq = means(idim, igaus) * means(idim, igaus);
	totdist += hefts[igaus] * distsq;
	distnorm += hefts[igaus] * distsq / dcovs(idim, igaus);
      }

      count ++;
    }

    distnorm /= count;
    
    if (max_distnorm == 0 || distnorm > max_distnorm)
    {
      best_hefts = hefts;
      best_means = means;
      best_dcovs = dcovs;

      cerr << "trial=" << itrial
	   << " distnorm old=" << max_distnorm << " new=" << distnorm
	   << " dist old=" << best_dist << " new=" << totdist
	   << " var old=" << best_var << " new=" << totvar << endl;

      max_distnorm = distnorm;
      best_dist = totdist;
      best_var = totvar;
    }
  }
  
  model->set_hefts (best_hefts);
  model->set_means (best_means);
  model->set_dcovs (best_dcovs);
}
catch (std::exception& error)
{
  throw Error (InvalidState, "brute_force_search_for_best_gmm", error.what());
}


void targeted_search_for_best_gmm (arma::gmm_diag* model,
				   const arma::mat& data,
				   unsigned ngaus, const double* eval) try
{
  arma::uword d = data.n_rows;     // dimensionality
  arma::uword N = data.n_cols;     // number of vectors

  // Tukey's default IQR threshold ...
  double threshold = 1.5;
  // ... results in about 1% false negatives in normal distribution
  double normal_outliers = 0.01;

  /* Half of these should be below Q1-1.5*IQR and half should be above
     Q3+1.5*IQR.  However, the number of outliers will be considered
     significant only when they are significantly times the false 
     negative rate of the normal distribution */

  double significantly = 1.5;

  unsigned outlier_count = significantly * normal_outliers/2 * N;

  cerr << "outlier_count=" << outlier_count << endl;

  // set the diagonal covariances to the eigenvalues
  arma::mat dcovs (d, ngaus);

  vector<double> row (N);
  arma::vec second (d); 
  double heft = 0;
  
  for (unsigned irow=0; irow < d; irow++)
  {
    for (unsigned igaus=0; igaus < ngaus; igaus++)
      dcovs (irow, igaus) = eval[irow];
    
    for (unsigned icol=0; icol < N; icol++)
      row[icol] =  data(irow, icol);

    sort (row.begin(), row.end());
    
    double Q1 = row[N/4];
    double Q3 = row[(3*N)/4];
    
    double IQR = Q3 - Q1;

    unsigned icol=0;
    while (row[icol] < Q1 - threshold*IQR)
      icol ++;

    unsigned nlow = icol;
    double lowmed = 0.0;
   
    if (nlow)
      lowmed = row[ nlow / 2 ];
   
    icol=N-1;
    while (row[icol] > Q3 + threshold*IQR)
     icol --;

    unsigned nhigh = N-1 - icol;
    double highmed = 0.0;

    if (nhigh)
      highmed = row[ icol + nhigh / 2 ];

#if _DEBUG
    cout << nlow << " " << lowmed
	 << " " << row[N/2] << " "
	 << nhigh << " " << highmed << endl;
#endif
    
    if (nlow > outlier_count && nlow > nhigh)
      second[irow] = lowmed;
    else if (nhigh > outlier_count)
      second[irow] = highmed;

    if (irow == 0)
      heft = double(max(nlow,nhigh)) / N;
  }

  unsigned km_iter = 0;
  unsigned em_iter = 0;
  bool verbose = false;

  double var_floor = 1e-10;

  // this fake run sets up the dimension inside the model
  // which is required in order to call model->set_means
  bool status = model->learn(data, ngaus, arma::maha_dist, arma::static_subset,
			     km_iter, em_iter, var_floor, verbose);
  
  if (status == false)
    throw Error (FailedCall, "CompareWith::setup",
		 "arma::gmm_diag::learn failed on fake run");

  cerr << "second = " << second[0] << " " << second[1] << endl;

  arma::mat means (d, ngaus);
  means.col(1) = second;

  arma::rowvec hefts (ngaus);

  double minheft = 0.01;
  hefts(0) = 1.0 - heft - (ngaus-2) * minheft;
  hefts(1) = heft;

  for (unsigned i=2; i<ngaus; i++)
    hefts(i) = minheft;
  
  model->set_means (means);
  model->set_hefts (hefts);
  model->set_dcovs (dcovs);
  
  em_iter = 40;

#if _DEBUG
  verbose = true;
#endif
  
  status = model->learn(data, ngaus, arma::maha_dist, arma::keep_existing,
			km_iter, em_iter, var_floor, verbose);

  if (status == false)
    throw Error (FailedCall, "CompareWith::setup",
		 "arma::gmm_diag::learn failed on targeted run");

}
catch (std::exception& error)
{
  throw Error (InvalidState, "targeted_search_for_best_gmm", error.what());
}

void ease_in_to_best_gmm (arma::gmm_diag* model,
			  const arma::mat& data,
			  unsigned& ngaus, const double* eval) try
{
  arma::uword d = data.n_rows;     // dimensionality
  arma::uword N = data.n_cols;     // number of vectors

  assert (d > 2);
  assert (N > 0);

  unsigned km_iter = 10;
  unsigned em_iter = 40;

  // initially fit only the first two principal components
  unsigned nrows = 2;
  arma::mat subdata = data.rows (0, nrows-1);

  bool verbose = false;

#if _DEBUG
  verbose = true;
#endif
  
  // determine the best number of Gaussians via the AIC
  ngaus = 1;
  unsigned best_ngaus = 1;
  double best_aic = 0;
  
  do
  {
    bool status = model->learn(subdata, ngaus, arma::maha_dist, arma::static_spread,
			       km_iter, em_iter, 1e-10, verbose);
    if (status == false)
      throw Error (FailedCall, "CompareWith::setup",
		   "arma::gmm_diag::learn failed on 2 PC run");

    double nparam = ngaus * ( 2 * nrows + 1 );
    double aic = nparam - model->sum_log_p (subdata);

    if (best_aic == 0 || aic < best_aic)
    {
      DEBUG( "ngaus=" << ngaus << " aic=" << aic << " best aic=" << best_aic);
      best_aic = aic;
      best_ngaus = ngaus;
      ngaus ++;
    }
    else
    {
      DEBUG( "BREAK: ngaus=" << ngaus << " aic=" << aic << " best aic=" << best_aic );
      break;
    }
  }
  while (ngaus == best_ngaus + 1);

  ngaus = best_ngaus;

  DEBUG( "number of Gaussians = " << ngaus );

  bool status = model->learn(subdata, ngaus, arma::maha_dist, arma::static_spread,
			       km_iter, em_iter, 1e-10, verbose);
  if (status == false)
    throw Error (FailedCall, "CompareWith::setup",
		 "arma::gmm_diag::learn failed on best 2 PC run");

#if _DEBUG
  model->hefts.print("hefts:");
  model->means.print("means:");
  model->dcovs.print("dcovs:");
#endif
  
  em_iter = 30;

  // add two more PCs at a time
  for (unsigned nrows=4; nrows < d; nrows+=2) try
  {
    subdata = data.rows (0, nrows-1);
    
    arma::rowvec hefts = model->hefts;    
    arma::mat means = model->means;
    arma::mat dcovs = model->dcovs;

    unsigned old_nrows = dcovs.n_rows;

    means.resize (nrows, ngaus);
    dcovs.resize (nrows, ngaus);
    
    for (unsigned irow=old_nrows; irow < nrows; irow++)
      for (unsigned icol=0; icol < ngaus; icol++)
      {
	means (irow, icol) = 0.0;
	dcovs (irow, icol) = eval[irow];
      }

    // fake run to set up model dimensions
    status = model->learn(subdata, ngaus, arma::maha_dist, arma::static_subset,
			  0, 0, 1e-10, false);

    if (status == false)
      throw Error (FailedCall, "CompareWith::setup",
		   "arma::gmm_diag::learn failed on fake nrow=%u", nrows);
    
    model->set_hefts (hefts);
    model->set_means (means);
    model->set_dcovs (dcovs);
    
    status = model->learn(subdata, ngaus, arma::maha_dist, arma::keep_existing,
			  0, em_iter, 1e-10, verbose);

    if (status == false)
      throw Error (FailedCall, "CompareWith::setup",
		   "arma::gmm_diag::learn failed on real nrow=%u", nrows);

    if (em_iter > 5)
      em_iter -= 2;

#if _DEBUG
    model->hefts.print("hefts:");
    model->means.print("means:");
    model->dcovs.print("dcovs:");
#endif
  }
  catch (std::exception& error)
  {
    throw Error (FailedCall, "ease_in_to_best_gmm",
                 error.what ());
  }
}
catch (std::exception& error)
{
  throw Error (InvalidState, "ease_in_to_best_gmm", error.what());
}

#endif // HAVE_ARMADILLO

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
  
  for (unsigned iprim=start_primary; iprim < start_primary+nprimary; iprim++)
  {
    (data->*primary) (iprim);
    
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

        var += 1.0;  // get_amps normalizes by robust rms
        norm += 1.0;
      }
      else
      {
        var += weight * robust_variance (amps);
        norm += weight;
      }
 
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

  if (eff_rank < 4)
  {
    float threshold = 1.5;

    while (eff_rank+1 < rank && eval[eff_rank]/eval[eff_rank+1] < threshold)
      eff_rank ++;

    cerr << "CompareWith::setup alternative effective rank=" 
         << eff_rank << endl;
  }

  if (eff_rank < 4)
  { 
    cerr << "CompareWith::setup work-around var=" << var 
         << " effective_rank=" << eff_rank 
         << " rank=" << rank << endl;
    double evaltot = 0;
    for (unsigned i=0; i<rank; i++)
      evaltot += eval[i];

    std::ofstream ofs ("eval_cumu.dat");
    ofs << "# eigenvalue cumulative" << endl;
    double eval_cumu = 0;
    for (unsigned i=0; i<rank; i++)
    {
      eval_cumu += eval[i];
      ofs << eval[i] << " " << 1.0 - eval_cumu / evaltot << endl;
    }

    cerr << "CompareWith::setup see eigenvalue data in eval_cumu.dat" << endl;
    eff_rank = rank - 1;
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

  // number of principal components passed to GMM on first iteration
  unsigned npc = eff_rank;

  initialize_armadillo_random_number_generator ();

  arma::gmm_diag* model = gmpd->model;
  if (model == NULL)
    model = gmpd->model = new arma::gmm_diag;
  
  arma::mat pc (npc, nprofile);

  // fill matrix with principal components after GCS fit

  unsigned iprofile = 0;

  std::vector<double> residual;

#define _OUTPUT_GMM_INPUT 0
#if _OUTPUT_GMM_INPUT
  std::ofstream ofs ("gmm_input.dat");
#endif
      
  for (unsigned iprim=start_primary; iprim < start_primary+nprimary; iprim++)
  {
    (data->*primary) (iprim);
    
    for (unsigned icompare=0; icompare < ncompare; icompare++)
    {
      (data->*compare) (icompare);

      Reference::To<const Profile> prof = data->get_Profile ();

      double weight = prof->get_weight();
      
      if (weight == 0.0)
	continue;

      vector<double> amps;
      get_amps (amps, prof);

      gcs->get (amps, mamps);

      residual = gcs->get_residual();
      if (npc < residual.size())
	residual.resize (npc);

#if _OUTPUT_GMM_INPUT
      for (unsigned i=0; i<npc; i++)
	ofs << residual[i] << " ";
      ofs << endl;
#endif
      
      pc.col(iprofile) = arma::vec( residual );
      iprofile ++;
    }
  }

  assert (iprofile == nprofile);
  pc.save ("gmm.sav");
  
  DEBUG( "CompareWith::setup calling arma::gmm_diag::learn" );

  /* The optimal number of Gaussians is determined by minimizing the
     information loss, as estimated via the Akaike Information
     Criterion */
  
  // number of Gaussians used to model data
  unsigned ncomponent = 0;  
  ease_in_to_best_gmm (model, pc, ncomponent, eval);

#if _DEBUG
  model->hefts.print("hefts:");
  model->means.print("means:");
#endif
  
  DEBUG( "CompareWith::setup Gaussian mixture analysis completed" );

#endif

}

void CompareWith::compute_mean (unsigned start_primary, unsigned nprimary)
{
  mean = 0;

  // DEBUG( "CompareWith::compute_mean start=" << start_primary << " n=" << nprimary);

  for (unsigned iprim=start_primary; iprim < start_primary+nprimary; iprim++)
  {
    (data->*primary) (iprim);
    
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
