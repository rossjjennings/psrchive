/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "SmoothingSpline.h"

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <cassert>

using namespace std;

CrossValidatedSmoothing::CrossValidatedSmoothing ()
{
  logarithmic = true;
  npartition = 80;
  validation_fraction = 0.2;
  ntrial = 30;
  spline = 0;
}

void CrossValidatedSmoothing::get_nfree_trials (vector<double>& nfree, unsigned ndat)
{
  unsigned nval = validation_fraction * ndat;
  unsigned nest = ndat - nval;
  unsigned nmin = 3;
  unsigned nmax = nest - 3;

  nfree.resize (ntrial);
  
  if (logarithmic)
  {
    double log_nmin = log(nmin);
    double log_nmax = log(nmax);
    double log_nfree_step = (log_nmax - log_nmin) / (ntrial - 1.0);
    for (unsigned itrial=0; itrial < ntrial; itrial ++)
      nfree[itrial] = exp( log_nmin + itrial * log_nfree_step );
  }
  else
  {
    double nfree_step = (nmax - nmin) / (ntrial - 1.0);
    for (unsigned itrial=0; itrial < ntrial; itrial ++)
      nfree[itrial] = nmin + itrial * nfree_step;
  }
}

void CrossValidatedSmoothing::fit ( vector< double >& dat_x,
				    vector< Estimate<double> >& dat_y )
{
  assert (spline != 0);

  // filter bad data
  unsigned idat=0;
  while (idat < dat_x.size())
  {
    if (dat_y[idat].var <= 0)
    {
      dat_x.erase (dat_x.begin() + idat);
      dat_y.erase (dat_y.begin() + idat);
    }
    else
      idat ++;
  }

  spline->set_minimize_gcv (true);
  spline->fit (dat_x, dat_y);

  unsigned ndat_good = spline->get_ndat_good();
  
  double best_fit_nfree = spline->get_fit_effective_nfree ();
  cerr << "CrossValidatedSmoothing::fit GCV best nfree="
       << best_fit_nfree << endl;

  vector<double> nfree_trials (ntrial);
  get_nfree_trials (nfree_trials, ndat_good);
  
  vector<double> mean_gof (ntrial, 0.0);
  
  for (unsigned itrial=0; itrial < ntrial; itrial ++)
  {
    spline->set_effective_nfree (nfree_trials[itrial]);

    mean_gof[itrial] = get_mean_gof (dat_x, dat_y);

#if _DEBUG
    cerr << "CrossValidatedSmoothing::fit nfree=" << nfree_trials[itrial]
	 << " gof=" << mean_gof[itrial] << endl;
#endif
  }

  unsigned imin = 0;
  double min_gof = mean_gof[0];

  for (unsigned itrial=1; itrial < ntrial; itrial++)
  {
    if (mean_gof[itrial] < min_gof)
    {
      min_gof = mean_gof[itrial];
      imin = itrial;
    }
  }

  double nfree = nfree_trials[imin];

  cerr << "CrossValidatedSmoothing::fit best cross nfree=" << nfree << endl;
  cerr << "RATIO: " << best_fit_nfree / nfree << endl;
  
  spline->set_effective_nfree (nfree);
  spline->fit (dat_x, dat_y);
}

static double sqr (double x) { return x*x; }

template<typename T>
void filter (vector<T>& to, const vector<T>& from, const vector<unsigned>& remove)
{
  assert (remove.size() < from.size());
  
  to.resize (from.size() - remove.size());

  unsigned current_remove = 0;
  unsigned current_to = 0;
  
  for (unsigned ifrom=0; ifrom < from.size(); ifrom++)
  {
    if (ifrom == remove[current_remove])
      current_remove++;
    else
    {
      to[current_to] = from[ifrom];
      current_to ++;
    }
  }

  assert (current_to == to.size());
  assert (current_remove == remove.size());
}

bool found (vector<unsigned>& vals, unsigned last, unsigned val)
{
  return vals.size() > 0 && last > 0
    && std::find (vals.begin(), vals.begin()+last, val) != vals.begin()+last;
}

void unique_sorted_random (vector<unsigned>& vals, unsigned nmax)
{
  unsigned nval = vals.size();
  
  for (unsigned ival=0; ival < nval; ival++)
  {
    unsigned trial = 0;
    
    do {
      trial = lrand48() % nmax;
    }
    while (found (vals, ival, trial));
    
    vals[ival] = trial;
  }

  sort (vals.begin(), vals.end());

#if _DEBUG
  cerr << "val: ";
  for (unsigned ival=0; ival < nval; ival++)
    cerr << vals[ival] << " ";
  cerr << endl;
#endif
}

double CrossValidatedSmoothing::get_mean_gof (const vector< double >& dat_x,
					      const vector< Estimate<double> >& dat_y)
{
  assert (spline != 0);

  srand48 (13);

  unsigned ndat = dat_x.size();
  unsigned nval = validation_fraction * ndat;
  vector<unsigned> validation_index (nval, 0);

  double total_gof = 0.0;
  
  for (unsigned ipart=0; ipart < npartition; ipart++)
  {
    unique_sorted_random (validation_index, ndat);
    
    vector< double > estimation_x;
    vector< Estimate<double> > estimation_y;

    filter (estimation_x, dat_x, validation_index);
    filter (estimation_y, dat_y, validation_index);

    spline->fit (estimation_x, estimation_y);

    for (unsigned ival=0; ival < nval; ival++)
    {
      double x = dat_x[ validation_index[ival] ];
      Estimate<double> y = dat_y[ validation_index[ival] ];
      total_gof += sqr(y.val - spline->evaluate(x)) / y.var;
    }
  }

  return total_gof / (nval * npartition);
}
