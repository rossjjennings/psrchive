/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Receiver.h"
#include "Pulsar/SingleAxis.h"
#include "Pulsar/SingleAxisSolver.h"
#include "Pulsar/Parallactic.h"
#include "Pauli.h"

using namespace std;

double min_cyclic (double diff)
{
  double min = diff;

  if (fabs(diff+2*M_PI) < fabs(diff)) 
    min = diff + 2*M_PI;

  if (fabs(diff-2*M_PI) < fabs(diff))
    min = diff - 2*M_PI;

  return min;
}

int main (int argc, char** argv) try
{
  bool verbose = false;

  /*
    simulate 21 on pulse phase bins, with PA = 0 in the eighth bin,
    and PA spanning 270 degrees
  */
  const unsigned nstate = 21;
  const unsigned zero = 7;

  Stokes<double> states [nstate];

  for (unsigned istate=0; istate < nstate; istate++)
  {
    double radian = M_PI*1.5 * double(int(istate)-int(zero))/nstate;
    states[istate] = Stokes<double> (1, cos(radian), sin(radian), 0);
  }

  Calibration::Parallactic para;
  Pulsar::Receiver receiver;
  Calibration::SingleAxis backend;

  Calibration::SingleAxisSolver solver;
  Calibration::SingleAxis solution;

  Estimate<double> one (1,1);
  Estimate<double> nil (0,1);
  Stokes< Estimate<double> > assume_cal (one, nil, one, nil);
  
  if (verbose)
    cerr << "assume cal=" << assume_cal << endl;

  cout << "\nhand\tcalU\tslope\ttrend\toffset" << endl;

  // start with right-handed receiver
  Signal::Hand hand = Signal::Right;

  for (unsigned i=0; i<2; i++)
  {
    receiver.set_hand (hand);
    Jones<double> actual_rcvr = receiver.get_transformation ();

    // start with cal Stokes U = 1
    Angle xyph = 0;

    for (unsigned j=0; j<2; j++)
    {
      receiver.set_reference_source_phase( xyph );
      Stokes<double> actual_cal = receiver.get_reference_source ();

      if (verbose)
	cerr << "actual cal=" << actual_cal << endl;

      unsigned up_count = 0;
      unsigned down_count = 0;

      unsigned pro_count = 0;
      unsigned con_count = 0;

      unsigned zero_count = 0;
      unsigned pi_count = 0;

      for (double phi=-M_PI*.99/2; phi < M_PI/2; phi += M_PI/20)
      {
	backend.set_diff_phase (phi);

	Jones<double> be = backend.evaluate();

	Stokes< Estimate<double> > obs_cal = transform (actual_cal, be);
	for (unsigned i=0; i<4; i++)
	  obs_cal[i].set_variance( 1.0 );

	if (verbose)
	  cerr << "observed cal=" << obs_cal << endl;

	solver.set_input (assume_cal);
	solver.set_output (obs_cal);
	solver.solve (&solution);

	double previous_zero = 500;

	for (double va = -M_PI/2; va <= M_PI/2; va += M_PI/30)
	{
	  para.set_parallactic_angle (va);
	  
	  Jones<double> R = para.evaluate ();

	  /*
	    The naive calibrator uses a solution based on the cal =
	    Stokes U assumption and the predicted parallactic angle
	    rotation.
	  */
	  Jones<double> calib = inv ( solution.evaluate() * R );

	  Jones<double> total = calib * be * actual_rcvr * R;
	  
	  double pa[nstate];
	  
	  for (unsigned istate=0; istate < nstate; istate++)
	  {
	    Stokes<double> result = transform( states[istate], total );
	    pa[istate] = atan2 (result[2], result[1]);
	  }

	  if (fabs(va) < 1e-15)
	  {
	    if (fabs(pa[zero]) < 1e-15)
	      zero_count ++;
	    if (fabs(min_cyclic (pa[zero]-M_PI)) < 1e-15)
	      pi_count ++;
	  }

	  /*
	    Count the number of times that the zero point is greater
	    than or less than the zero point from the previous simulated
	    parallactic angle.  This is used to describe the PA trend
	    as a function of parallactic angle.
	  */
	  if (previous_zero != 500)
	  {
	    double diff = min_cyclic (pa[zero] - previous_zero);
	      
	    if (diff > 1e-15)
	      pro_count ++;
	      
	    if (diff < -1e-15)
	      con_count ++;

	    // cerr << "pa=" << pa[zero] << " va=" << va << endl;
	  }

	  previous_zero = pa[zero];

	  for (unsigned istate = 1; istate < nstate; istate++)
	  {
	    double diff = min_cyclic (pa[istate] - pa[istate-1]);
	      
	    if (diff > 0)
	      up_count ++;
	      
	    if (diff < 0)
	      down_count ++;
	  }
	}
      }

      string slope = "undef";

      if (up_count && !down_count)
	slope = "up";
      if (!up_count && down_count)
	slope = "down";

      string trend = "none";

      if (pro_count && !con_count)
	trend = "pro";
      if (!pro_count && con_count)
	trend = "con";
      if (pro_count && con_count)
	trend = "mix";

      string pi = "undef";

      if (zero_count && !pi_count)
	pi = "0";
      if (!zero_count && pi_count)
	pi = "pi";

      cout << hand << "\t" << 1-int(j)*2 
	   << "\t" << slope << "\t" << trend << "\t" << pi << endl;

      // switch to cal Stokes U = -1
      xyph = M_PI;
    }

    // switch to left-handed receiver
    hand = Signal::Left;

  }
  return 0;
}
catch (Error& error)
{
  cerr << error << endl;
  return -1;
}
