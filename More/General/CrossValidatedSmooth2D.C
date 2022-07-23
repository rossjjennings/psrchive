/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SplineSmooth.h"
#include "UnaryStatistic.h"
#include "EstimateStats.h"
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

  // 5-fold cross-validation by default
  npartition = 5;
  validation_fraction = 0.2;

  iqr_threshold = 2;
  gof_step_threshold = 1.0;
  
  spline = 0;
  gof_out = 0;

  nflagged_iqr = 0;
  nflagged_gof = 0;
}

void CrossValidatedSmooth2D::remove_iqr_outliers
( vector< pair<double,double> >& dat_x,
  vector< Estimate<double> >& dat_y )
{
  if (iqr_threshold <= 0)
    return;

  unsigned ndat = dat_x.size();
  assert (ndat == dat_y.size());

  Estimate<double> Q1, Q2, Q3;
  weighted_quartiles (dat_y, Q1, Q2, Q3);

  double IQR = Q3.val - Q1.val;
  double max_threshold = Q3.val + iqr_threshold * IQR;
  double min_threshold = Q1.val - iqr_threshold * IQR;
  
  nflagged_iqr = 0;
  unsigned count = 0;
  for (unsigned idat=0; idat < ndat; idat++)
  {
    if (dat_y[idat].var <= 0)
      continue;

    count ++;
    double val = dat_y[idat].val;
    
    if (val > max_threshold || val < min_threshold)
    {
      // cerr << "IQR outlier idat=" << idat << " val=" << val << endl;
      dat_y[idat].var = 0.0;
      nflagged_iqr++;
    }
  }
  
  cerr << "CrossValidatedSmooth2D::remove_iqr_outliers"
    " flagged " << nflagged_iqr << " out of " << count << " values" << endl;
}

void CrossValidatedSmooth2D::fit ( vector< pair<double,double> >& dat_x,
				   vector< Estimate<double> >& dat_y )
{
  assert (spline != 0);

  unsigned ndat = dat_x.size();
  assert (ndat == dat_y.size());

  if (dat_x.size() == 0)
    return;

  if (iqr_threshold)
    remove_iqr_outliers (dat_x, dat_y);
  else
    nflagged_iqr = 0;

  find_optimal_smoothing_factor (dat_x, dat_y);

  if (gof_step_threshold)
    remove_gof_outliers (dat_x, dat_y);
  else
    nflagged_gof = 0;
  
  spline->fit (dat_x, dat_y);
}

void CrossValidatedSmooth2D::remove_gof_outliers
( vector< pair<double,double> >& dat_x,
  vector< Estimate<double> >& dat_y )
{
  unsigned ndat = dat_x.size();
  assert (ndat == dat_y.size());
  
  assert (ndat == gof_tot.size());
  assert (ndat == gof_count.size());
  
  for (unsigned idat=0; idat < ndat; idat++)
  {
    if (gof_count[idat] > 0)
      gof_tot[idat] /= gof_count[idat];
  }

  vector<double> tmp = gof_tot;
  
  std::sort (tmp.begin(), tmp.end());

  double threshold = 0;
  
  for (unsigned i=0; i+1 < ndat; i++)
    if ((tmp[i+1]-tmp[i]) > gof_step_threshold)
    {
      threshold = tmp[i] + 0.1;
      break;
    }
  
  // cerr << "outlier threshold=" << threshold << endl;
  
  std::ofstream os ("gof.dat");
  
  unsigned count = 0;
  nflagged_gof = 0;

  for (unsigned idat=0; idat < ndat; idat++)
  {
    os << tmp[idat] << endl;

    if (gof_count[idat] == 0)
      continue;

    count ++;
    
    if (threshold && gof_tot[idat] > threshold)
    {
      // cerr << "GOF outlier idat=" << idat << " gof=" << gof_tot[idat] << endl;
      dat_y[idat].var = 0.0;
      nflagged_gof ++;
    }
  }

  cerr << "CrossValidatedSmooth2D::remove_gof_outliers"
    " flagged " << nflagged_gof << " out of " << count << " values" << endl;
}

double CrossValidatedSmooth2D::get_mean_gof
( double log_10_alpha,
  const vector< pair<double,double> >& dat_x,
  const vector< Estimate<double> >& dat_y )
{
  double alpha = pow (10.0, log_10_alpha);
  spline->set_alpha (alpha);
  return get_mean_gof (dat_x, dat_y);
}

void CrossValidatedSmooth2D::find_optimal_smoothing_factor
( const vector< pair<double,double> >& dat_x,
  const vector< Estimate<double> >& dat_y )
{
  unsigned ndat = dat_x.size();
  assert (ndat == dat_y.size());
  
  gof_tot.resize (ndat);
  gof_count.resize (ndat);

  for (unsigned idat=0; idat < ndat; idat++)
  {
    gof_tot[idat] = 0;
    gof_count[idat] = 0;
  }

  if (gof_filename != "")
  {
    gof_out = new ofstream (gof_filename.c_str());
 
    (*gof_out) << "# log10_smoothing_factor mean_gof median_gof" << endl;
  }

  double val[3] = { 3, 0, 6 };
  double gof[3] = { 0, 0, 0 };
  double close_enough = 0.1;

  // midpoint
  val[1] = (val[2] + val[0]) / 2;
        
  for (unsigned ival=0; ival < 3; ival++)
  {
    gof[ival] = get_mean_gof (val[ival], dat_x, dat_y);
    // cerr << ival << " " << val[ival] << " " << gof[ival] << endl;
  }

  double interval = val[2] - val[0];
  
  while (gof[0] < gof[1])
  {
    // the solution lies to the left
    // cerr << "go left of " << val[0] << " by " << interval << endl;

    val[2] = val[1];
    gof[2] = gof[1];
      
    val[1] = val[0];
    gof[1] = gof[0];
      
    val[0] -= interval;
    gof[0] = get_mean_gof (val[0], dat_x, dat_y);

    interval = val[2] - val[0];
  }
  
  while (gof[2] < gof[1])
  {
    // the solution lies to the right
    // cerr << "go right of " << val[2] << " by " << interval << endl;
    val[0] = val[1];
    gof[0] = gof[1];

    val[1] = val[2];
    gof[1] = gof[2];

    val[2] += interval;
    gof[2] = get_mean_gof (val[2], dat_x, dat_y);

    interval = val[2] - val[0];
  }

  // binary search for the optimal smoothing factor
  while (interval > close_enough)
  {
    // cerr << "*** interval=" << interval << endl;

#if _DEBUG
    for (unsigned ival=0; ival < 3; ival++)
      cerr << ival << " " << val[ival] << " " << gof[ival] << endl;
#endif
    
    if (gof[0] < gof[2])
    {
      double new_interval = val[2] - val[1];
      double new_point = val[1] + 0.25 * new_interval;
      // cerr << "divide right half at " << new_point << endl;

      double new_gof = get_mean_gof (new_point, dat_x, dat_y);

      if (new_gof < gof[1])
      {
	// cerr << "shift to right half" << endl;
	val[0] = val[1];
	gof[0] = gof[1];

	val[1] = new_point;
	gof[1] = new_gof;
      }
      else
      {
	// cerr << "shorten right half" << endl;
	val[2] = new_point;
	gof[2] = new_gof;
      }
    }
    else
    {
      double new_interval = val[0] - val[1];
      double new_point = val[1] + 0.25 * new_interval;

      // cerr << "divide left half at " << new_point << endl;

      double new_gof = get_mean_gof (new_point, dat_x, dat_y);

      if (new_gof < gof[1])
      {
	// cerr << "shift to left half" << endl;
	val[2] = val[1];
	gof[2] = gof[1];

	val[1] = new_point;
	gof[1] = new_gof;
      }
      else
      {
	// cerr << "shorten left half" << endl;
	val[0] = new_point;
	gof[0] = new_gof;
      }
    }

    interval = val[2] - val[0];
  }

  // cerr << "*** done.  last fit." << endl;
  
  double alpha = pow (10.0, val[1]);
  spline->set_alpha (alpha);
  spline->fit (dat_x, dat_y);

  if (gof_out)
    delete gof_out;
}


static double sqr (double x) { return x*x; }

template<typename T, typename U>
void flag (vector< Estimate<T,U> >& data, const vector<unsigned>& remove)
{
  unsigned ndat = data.size();
  assert (remove.size() < ndat);
  
  for (unsigned i=0; i < remove.size(); i++)
  {
    unsigned idat = remove[i];
    assert (idat < ndat);
    data[idat].var = 0.0;
  }
}

void unique_sorted_random (vector<unsigned>& vals,
			   vector<unsigned>& used_count,
			   unsigned& use_if)
{
  unsigned nval = vals.size();
  unsigned nmax = used_count.size();

  unsigned total_available = 0;
  for (unsigned ival=0; ival < nmax; ival++)
    if (used_count[ival] <= use_if)
      total_available ++;

  // cerr << "total_available=" << total_available << " nval=" << nval << " use_if=" << use_if << endl;
  
  unsigned ival = 0;

  vector<bool> used (used_count.size(), false);
  
  if (total_available <= nval)
  {
    for (unsigned jval=0; jval < nmax; jval++)
    {
      if (used_count[jval] <= use_if)
	{
	  vals[ival] = jval;
	  ival ++;
	  used_count[jval] ++;
	  used[jval] = true;
	}
    }
    
    use_if ++;
  }
  
  while (ival < nval)
  {
    unsigned trial = 0;
    
    do {
      trial = lrand48() % nmax;
    }
    while (used[trial] || used_count[trial] > use_if );

    vals[ival] = trial;
    used_count[trial] ++;
    used[trial] = true;
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

double CrossValidatedSmooth2D::get_mean_gof
(const vector< pair<double,double> >& dat_x,
 const vector< Estimate<double> >& dat_y)
{
  assert (spline != 0);

  srand48 (13);

  unsigned ndat = dat_x.size();
  unsigned nval = ceil (validation_fraction * ndat);

  // cerr << "CrossValidatedSmooth2D::get_mean_gof ndat=" << ndat << " nval=" << nval << endl;
  
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
    // cerr << "calling unique_sorted_random" << endl;
    unique_sorted_random (validation_index, used_count, use_if);
    // cerr << "unique_sorted_random done" << endl;
    
    vector< Estimate<double> > estimation_y = dat_y;
    flag (estimation_y, validation_index);
    
    // cerr << "spline fit" << endl;
    spline->fit (dat_x, estimation_y);
    // cerr << "fit done" << endl;

    unsigned vval = 0;
    for (unsigned ival=0; ival < ndat; ival++)
    {
      bool validation = false;
      
      if (vval < nval && ival == validation_index[vval])
      {
	validation = true;
	vval ++;
      }
      
      pair<double,double> x = dat_x[ ival ];
      Estimate<double> y = dat_y[ ival ];

      if (y.var == 0.0)
	continue;
      
      double gof = sqr(y.val - spline->evaluate(x)) / y.var;

      if (validation)
      {
	total_gof += gof;
	count ++;
      
	validation_gof[ ival ] += gof;
	validation_count[ ival ] ++;
      }
      else
      {
	estimation_gof[ ival ] += gof;
	estimation_count[ ival ] ++;
      }
    }

    assert (vval == nval);
  }

  
  for (unsigned idat=0; idat < ndat; idat++)
  {
    if (dat_y[idat].var == 0.0)
      continue;

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
  filtered_Q1_Q2_Q3 (validation_gof, Q1, Q2, Q3, 0.0);
  // double IQR = Q3 - Q1;

  if (gof_out)
    (*gof_out) << log10(spline->get_alpha()) << " " << mean_gof << " " << Q2 << endl;
  
  // cerr << "validation Q1=" << Q1 << " Q2=" << Q2 << " Q3=" << Q3 << " IQR=" << IQR << endl;
  
  if (Q2 <= 1.0)
  {
    double Q1, Q2, Q3;
    filtered_Q1_Q2_Q3 (estimation_gof, Q1, Q2, Q3, 0.0);
    double IQR = Q3 - Q1;
    
    // cerr << "estimation Q1=" << Q1 << " Q2=" << Q2 << " Q3=" << Q3 << " IQR=" << IQR << endl;

    for (unsigned idat=0; idat < ndat; idat++)
    {
      if (estimation_count[idat] == 0)
	continue;

      double gof = (estimation_gof[idat] - Q2) / IQR;
      gof_tot[idat] += gof;
      gof_count[idat] ++;
    }
  }
  
  // cerr << "gof median=" << Q2 << " mean=" << mean_gof << endl;
  
  return mean_gof;
}
