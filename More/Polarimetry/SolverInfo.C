/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SolverInfo.h"
#include "Pulsar/PolnCalibrator.h"

//! Constructor
Pulsar::SolverInfo::SolverInfo (const PolnCalibrator* cs)
{
  poln_calibrator = cs;
}
    
std::string Pulsar::SolverInfo::get_title () const
{
  return "Goodness of Fit (Reduced \\gx\\u2\\d)";
}

unsigned Pulsar::SolverInfo::get_nchan () const
{
  return poln_calibrator->get_nchan();
}

unsigned Pulsar::SolverInfo::get_nclass () const
{
  return 1;
}

std::string Pulsar::SolverInfo::get_label (unsigned iclass) const
{
  return "\\gx\\u2\\d/N\\dfree";  // chi^2 / N_{free}
}

unsigned Pulsar::SolverInfo::get_nparam (unsigned iclass) const
{
  return 1;
}

Estimate<float> 
Pulsar::SolverInfo::get_param (unsigned ichan, unsigned iclass, unsigned iparam) const
{
  if (!poln_calibrator->get_transformation_valid(ichan))
    return 0.0;

  const MEAL::LeastSquares* solver = poln_calibrator->get_solver (ichan);

  if (!solver->get_solved() || !solver->get_nfree())
    return 0.0;

  double reduced_chisq = solver->get_chisq() / solver->get_nfree();
  double variance = 1.0 / solver->get_nfree();

  return Estimate<float> (reduced_chisq, variance);
}

