/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SplineSmooth.h"
#include "UnaryStatistic.h"
#include "pairutil.h"

#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <cassert>

#include <bspline_utils.h>

using namespace std;
using namespace Pulsar;

CrossValidatedSmooth2D::CrossValidatedSmooth2D ()
{
  logarithmic = true;
  npartition = 5;
  validation_fraction = 0.2;
  spline = 0;
}

#if 0
void CrossValidatedSmooth2D::get_nfree_trials (vector<double>& nfree, unsigned ndat)
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

#endif

/*!

  The 
 */

using namespace SPLINTER;

// extern SparseMatrix compute_second_order_finite_difference_matrix(const BSpline &bspline);

void CrossValidatedSmooth2D::fit (const vector< pair<double,double> >& dat_x,
				   const vector< Estimate<double> >& dat_y)
{
  assert (spline != 0);

  unsigned ndat = dat_x.size();
  assert (ndat == dat_y.size());

  if (dat_x.size() == 0)
    return;

  double val[3] = { 3, 0, 6 };
  double gof[3] = {  0, 0,  0 };
  double close_enough = 0.1;

  double interval = val[2] - val[0];
  double multiplier = 2.0;
  int direction = 0;
  
  gof_tot.resize (ndat);
  gof_count.resize (ndat);

  for (unsigned idat=0; idat < ndat; idat++)
  {
    gof_tot[idat] = 0;
    gof_count[idat] = 0;
  }

  // binary search for the optimal smoothing factor
  while (interval > close_enough)
  {
    // midpoint
    val[1] = (val[2] + val[0]) / 2;
    gof[1] = 0.0;
    
    for (unsigned ival=0; ival < 3; ival++)
    {
      if (gof[ival] == 0.0)
      {
	double alpha = pow (10.0, val[ival]);
	spline->set_alpha (alpha);
	gof[ival] = get_mean_gof (dat_x, dat_y);

	cerr << ival << " " << val[ival] << " " << gof[ival] << endl;
      }
    }
    
    if (gof[0] < gof[1])
    {
      if (multiplier == 2.0 && direction == 1.0)
	multiplier = 0.5;

      direction = -1.0;
      
      // the solution lies to the left
      cerr << "go left of " << val[0] << " by " << interval*multiplier << endl;

      val[2] = val[0];
      gof[2] = gof[0];
      
      val[0] -= interval * multiplier;
      gof[0] = 0;
    }
    else if (gof[2] < gof[1])
    {
      if (multiplier == 2.0 && direction == -1.0)
	multiplier = 0.5;

      direction = 1.0;
      
      // the solution lies to the right
      cerr << "go right of " << val[2] << " by " << interval*multiplier << endl;
      val[0] = val[2];
      gof[0] = gof[2];

      val[2] += interval * multiplier;
      gof[2] = 0;
    }
    else
    {
      if (gof[0] < gof[2])
      {
	cerr << "left half" << endl;
	val[2] = val[1];
	gof[2] = gof[1];
	multiplier = 0.5;
      }
      else
      {
	cerr << "right half" << endl;
	val[0] = val[1];
	gof[0] = gof[1];
	multiplier = 0.5;
      }
    }

    interval = val[2] - val[0];
    cerr << "interval=" << interval << " val[2]=" << val[2] << " val[0]=" << val[0] << endl;
    
  }

  for (unsigned idat=0; idat < ndat; idat++)
  {
    if (gof_count[idat] == 0)
      continue;

    gof_tot[idat] /= gof_count[idat];
  }

  vector<double> tmp = gof_tot;
  
  std::sort (tmp.begin(),tmp.end());

  double threshold = 0;
  
  for (unsigned i=0; i+1 < ndat; i++)
    if ((tmp[i+1]-tmp[i]) > 1.0)
    {
      threshold = tmp[i] + 0.1;
      break;
    }
  
  cerr << "outlier threshold=" << threshold << endl;
  
  std::ofstream os ("gof.dat");

  unsigned outliers = 0;

  vector< pair<double,double> > tmp_x = dat_x;
  vector< Estimate<double> > tmp_y = dat_y;
    
  for (unsigned idat=0; idat < ndat; idat++)
  {
    os << tmp[idat] << endl;
    
    if (threshold && gof_tot[idat] > threshold)
    {
      cerr << "outlier idat=" << idat << " gof=" << gof_tot[idat] << endl;
      tmp_x.erase (tmp_x.begin() + idat - outliers);
      tmp_y.erase (tmp_y.begin() + idat - outliers);

      outliers ++;
    }
  }

  if (outliers)
    spline->fit (tmp_x, tmp_y);
}

static double sqr (double x) { return x*x; }

template<typename Tx, typename Ty>
void filter (vector<Tx>& to, const vector<Ty>& from, const vector<unsigned>& remove)
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

void unique_sorted_random (vector<unsigned>& vals,
			   vector<unsigned>& used, unsigned& use_if)
{
  unsigned nval = vals.size();
  unsigned nmax = used.size();

  unsigned total_available = 0;
  for (unsigned ival=0; ival < nmax; ival++)
    if (used[ival] <= use_if)
      total_available ++;

  //cerr << "total_available=" << total_available << " nval=" << nval << endl;
  
  unsigned ival = 0;
  
  if (total_available <= nval)
  {
    for (unsigned jval=0; jval < nmax; jval++)
    {
      if (used[jval] <= use_if)
	{
	  vals[ival] = jval;
	  ival ++;
	  used[jval] ++;
	}
    }
    use_if ++;

    //cerr << "after using remaining, use_if=" << use_if << endl;
  }
  
  while (ival < nval)
  {
    unsigned trial = 0;
    
    do {
      trial = lrand48() % nmax;
    }
    while (used[trial] > use_if);

    vals[ival] = trial;
    used[trial] ++;
    ival++;
  }

  sort (vals.begin(), vals.end());

#if _DEBUG
  cerr << "val: ";
  for (unsigned ival=0; ival < nval; ival++)
    cerr << vals[ival] << " ";
  cerr << endl;
#endif
}

double CrossValidatedSmooth2D::get_mean_gof (const vector< pair<double,double> >& dat_x,
					      const vector< Estimate<double> >& dat_y)
{
  assert (spline != 0);

  srand48 (13);

  unsigned ndat = dat_x.size();
  unsigned nval = ceil (validation_fraction * ndat);
  vector<unsigned> validation_index (nval, 0);
  vector<unsigned> used_count (ndat, 0);
  unsigned use_if = 0;
  
  double total_gof = 0.0;
  unsigned count = 0;
  
  vector<double> validation_gof (ndat, 0.0);
  vector<unsigned> validation_count (ndat, 0);
  
  vector<double> estimation_gof (ndat, 0.0);
  vector<unsigned> estimation_count (ndat, 0);
  
  for (unsigned ipart=0; ipart < npartition; ipart++)
  {
    unique_sorted_random (validation_index, used_count, use_if);
    
    vector< pair<double,double> > estimation_x;
    vector< Estimate<double> > estimation_y;

    filter (estimation_x, dat_x, validation_index);
    filter (estimation_y, dat_y, validation_index);

    spline->fit (estimation_x, estimation_y);

    unsigned vval = 0;
    for (unsigned ival=0; ival < ndat; ival++)
    {
      unsigned jval = ival;
      bool validation = false;
      
      if (vval < nval && ival == validation_index[vval])
      {
	jval = validation_index[vval];
	validation = true;
	vval ++;
      }
      
      pair<double,double> x = dat_x[ jval ];
      Estimate<double> y = dat_y[ jval ];

      double gof = sqr(y.val - spline->evaluate(x)) / y.var;

      if (validation)
      {
	total_gof += gof;
	count ++;
      
	validation_gof[ jval ] += gof;
	validation_count[ jval ] ++;
      }
      else
      {
	estimation_gof[ jval ] += gof;
	estimation_count[ jval ] ++;
      }
    }

    assert (vval == nval);
  }

  
  for (unsigned idat=0; idat < ndat; idat++)
  {
    if (validation_count[idat] == 0)
      cerr << idat << " not selected in validation partition" << endl;
    else
      validation_gof[idat] /= validation_count[idat];

    if (estimation_count[idat] > 0)
      estimation_gof[idat] /= estimation_count[idat];
  }

  // cerr << "count=" << count << " norm=" << nval * npartition << endl;
  
  double mean_gof = total_gof / count;

  double Q1, Q2, Q3;
  Q1_Q2_Q3 (validation_gof, Q1, Q2, Q3);
  double IQR = Q3 - Q1;

  cerr << "Q1=" << Q1 << " Q2=" << Q2 << " Q3=" << Q3 << " IQR=" << IQR << endl;
  
  if (Q2 <= 1.0)
  {
    double Q1, Q2, Q3;
    Q1_Q2_Q3 (estimation_gof, Q1, Q2, Q3);
    double IQR = Q3 - Q1;
    
    cerr << "estimation Q1=" << Q1 << " Q2=" << Q2 << " Q3=" << Q3 << " IQR=" << IQR << endl;

    for (unsigned idat=0; idat < ndat; idat++)
    {
      if (estimation_count[idat] == 0)
	continue;

      double gof = (estimation_gof[idat] - Q2) / IQR;
      gof_tot[idat] += gof;
      gof_count[idat] ++;
    }
  }
  
  cerr << "gof median=" << Q2 << " mean=" << mean_gof << endl;
  
  return Q2;
}
