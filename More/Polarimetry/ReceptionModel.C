/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/ReceptionModel.h"
#include "Pulsar/CoherencyMeasurementSet.h"

#include <assert.h>

using namespace std;

// #define _DEBUG 1

Calibration::ReceptionModel::ReceptionModel ()
{
  // name = "ReceptionModel";

  // should be ok
  maximum_iterations = 50;

  // Relative \Delta\chi^2 considered significant
  convergence_threshold = 1e-2;

  // By default, do not cull solutions
  maximum_reduced = 0.0;

  // By default, not simulated
  exact_solution = false;

  // Switch between ReceptionModel:: and MeasurementEquation::calculate
  top_calculate = false;

  // set during solve
  iterations = 0;
  best_chisq = 0.0;
  nfree = 0;

  // used during solve
  fit_debug = false;
}

Calibration::ReceptionModel::~ReceptionModel ()
{
}

//! Return the name of the class
string Calibration::ReceptionModel::get_name () const
{
  return "ReceptionModel";
}

//! The number of iterations in last call to solve method
unsigned Calibration::ReceptionModel::get_fit_iterations () const
{
  return iterations;
}

//! The chi-squared in last call to solve method
float Calibration::ReceptionModel::get_fit_chisq () const
{
  return best_chisq;
}

//! The number of free parameters in last call to solve method
unsigned Calibration::ReceptionModel::get_fit_nfree () const
{
  return nfree;
}

//! Additional input, \f$\rho_i,k\f$, where \f$i\f$=transformation_index
void Calibration::ReceptionModel::add_to_output (unsigned index,
                                                 MEAL::Complex2* rho)
{
  if (index >= xforms.size())
    throw Error (InvalidRange, 
		 "Calibration::ReceptionModel::add_to_output",
		 "index=%d >= npath=%d", index, xforms.size());

  if (!top_calculate) {
    for (unsigned ipath=0; ipath < xforms.size(); ipath++) {
      output.push_back( MEAL::SumRule<MEAL::Complex2>() );
      output.back() += this;
    }
    top_calculate = true;
  }

  assert (xforms.size() == output.size());

  if (!rho)
    return;

  output[index] += rho;

  if (index == current_xform)
    set_evaluation_changed();
}

//! Add a transformation, \f$J_M\f$, where \f$M\f$ = get_num_transformation
void Calibration::ReceptionModel::add_transformation (MEAL::Complex2* state)
{
  if (top_calculate) {
    assert (xforms.size() == output.size());
    output.push_back( MEAL::SumRule<MEAL::Complex2>() );
    output.back() += this;
  }

  MeasurementEquation::add_transformation (state);
}

//! Returns \f$ \rho^\prime \f$ and its gradient
void Calibration::ReceptionModel::calculate (Jones<double>& result, 
					     vector<Jones<double> >* grad)
{
  if (top_calculate) {

    if (verbose)
      cerr << "Calibration::ReceptionModel::calculate top current_xform="
	   << current_xform << endl;
    
    top_calculate = false;
    result = output[current_xform].evaluate (grad);
    top_calculate = true;

  }
  else {

    if (verbose) cerr << "Calibration::ReceptionModel::calculate"
		   " call MeasurementEquation::calculate" << endl;
    MeasurementEquation::calculate (result, grad);

  }
}

//! Checks that each MeasuredState has a valid source_index
void Calibration::ReceptionModel::add_data (CoherencyMeasurementSet& datum)
{
  if (datum.get_transformation_index() >= get_num_transformation())
      throw Error (InvalidParam,
		   "Calibration::ReceptionModel::add_data",
		   "path_index=%d >= npath=%d",
		   datum.get_transformation_index(), get_num_transformation());
 
  for (unsigned imeas=0; imeas<datum.size(); imeas++)  {

    if (datum[imeas].get_input_index() >= get_num_input())
      throw Error (InvalidParam,
		   "Calibration::ReceptionModel::add_data",
		   "source_index=%d >= nsource=%d",
		   datum[imeas].get_input_index(), get_num_input());

#if 0
    for (unsigned ipol=0; ipol<4; ipol++)
      if (datum[imeas].get_variance(ipol) <= 0.0) {
	Error error (InvalidParam, "Calibration::ReceptionModel::add_data");
	error << "source_index=" << datum[imeas].get_input_index()
	      << " ipol=" << ipol 
	      << " variance=" << datum[imeas].get_variance(ipol) << " <= 0";
        if (verbose)
          error << "\nstokes=" << datum[imeas].get_stokes();
        throw error;
      }
#endif

  }

  data.push_back (datum);
}

void Calibration::ReceptionModel::delete_data ()
{
  data.resize (0);
}


//! Get the number of CoherencyMeasurementSet
unsigned Calibration::ReceptionModel::get_ndata () const
{
  return data.size();
}

//! Get the specified CoherencyMeasurementSet
const Calibration::CoherencyMeasurementSet&
Calibration::ReceptionModel::get_data (unsigned idata) const
{
  range_check (idata, "Calibration::ReceptionModel::get_data");
  return data[idata];
}

void Calibration::ReceptionModel::range_check (unsigned idata, 
					       const char* method) const
{
  if (idata >= data.size())
    throw Error (InvalidRange, method, "idata=%d >= ndata=%d",
		 idata, data.size());
}

//! Set the maximum number of iterations in fit algorithm
void Calibration::ReceptionModel::set_fit_maximum_iterations (unsigned max)
{
  maximum_iterations = max;
}

//! Set the convergence threshold
void Calibration::ReceptionModel::set_fit_convergence_threshold (float ct,
								 bool exact)
{
  convergence_threshold = ct;
  exact_solution = exact;
}

//! Get the covariance matrix of the last fit
void 
Calibration::ReceptionModel::get_fit_covariance (vector<vector<double> >& c)
  const
{
  c = covariance;
}
