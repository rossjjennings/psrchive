/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "SmoothingSpline.h"
#include <iostream>
#include <cstdlib>
#include <cassert>

using namespace std;

CrossValidatedSmoothing::CrossValidatedSmoothing ()
{
  npartition = 40;
  validation_fraction = 0.1;
  ntrial = 30;
  spline = 0;
}

void CrossValidatedSmoothing::fit (const vector< double >& dat_x,
				   const vector< Estimate<double> >& dat_y)
{
  assert (spline != 0);
  
  spline->set_minimize_gcv (true);
  spline->fit (dat_x, dat_y);

  cerr << "CrossValidatedSmoothing::fit GCV best nfree="
       << spline->get_fit_effective_nfree () << endl;

  unsigned ndat = dat_x.size();
  unsigned nval = validation_fraction * ndat;
  unsigned nest = ndat - nval;
  unsigned nmin = 3;
  
  double nfree_step = (nest - nmin) / (ntrial + 1.0);

  cerr << "CrossValidatedSmoothing::fit nfree_step="
       << nfree_step << endl;
  
  vector<double> mean_gof (ntrial, 0.0);
  
  for (unsigned itrial=0; itrial < ntrial; itrial ++)
  {
    double nfree = nmin + itrial * nfree_step;

    spline->set_effective_nfree (nfree);

    mean_gof[itrial] = get_mean_gof (dat_x, dat_y);

    cerr << "CrossValidatedSmoothing::fit nfree=" << nfree
	 << " gof=" << mean_gof[itrial] << endl;
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

  double nfree = nmin + imin * nfree_step;

  cerr << "CrossValidatedSmoothing::fit best nfree=" << nfree << endl;
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
