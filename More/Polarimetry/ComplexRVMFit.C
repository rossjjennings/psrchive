/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ComplexRVMFit.h"
#include "Pulsar/PolnProfile.h"
#include "Pulsar/PhaseWeight.h"

#include "MEAL/ComplexRVM.h"
#include "MEAL/LevenbergMarquardt.h"

#include <assert.h>

using namespace std;

Pulsar::ComplexRVMFit::ComplexRVMFit()
{
  threshold = 3.0;
}

//! Set the threshold below which data are ignored
void Pulsar::ComplexRVMFit::set_threshold (float sigma)
{
  threshold = sigma;
}

//! Get the threshold below which data are ignored
float Pulsar::ComplexRVMFit::get_threshold () const
{
  return threshold;
}

//! Set the data to which model will be fit
void Pulsar::ComplexRVMFit::set_observation (const PolnProfile* _data)
{
  if (_data->get_state() != Signal::Stokes)
    throw Error (InvalidParam, "Pulsar::ComplexRVMFit::set_observation"
		 "data are not Stokes parameters");

  data = _data;

  if (model->get_nstate())
    model = 0;
}

//! Get the data to which model will be fit
const Pulsar::PolnProfile* Pulsar::ComplexRVMFit::get_observation () const try
{
  return data;
}
catch (Error& error)
{
  throw error += "Pulsar::ComplexRVMFit::get_observation";
}

//! Get the model to be fit to the data
MEAL::ComplexRVM* Pulsar::ComplexRVMFit::get_model ()
{
  if (!model)
    model = new MEAL::ComplexRVM;

  return model;
}

//! Fit data to the model
void Pulsar::ComplexRVMFit::solve ()
{
  if (!model)
    model = new MEAL::ComplexRVM;

  std::vector< std::complex< Estimate<double> > > linear;
  data->get_linear (linear, threshold);

  const unsigned nbin = data->get_nbin();

  MEAL::Axis<unsigned> state;

  std::vector< MEAL::Axis<unsigned>::Value > data_x;
  std::vector< std::complex< Estimate<double> > > data_y;

  unsigned count = 0;

  for (unsigned ibin=0; ibin < nbin; ibin++)
    if (linear[ibin].real().get_variance() > 0)
    {
      data_x.push_back ( state.get_Value(count) );
      data_y.push_back ( linear[ibin] );

      double phase = (ibin + 0.5)*2*M_PI / nbin;
      double L = sqrt( norm(linear[ibin]).val );

      cerr << "adding phase=" << phase << " L=" << L << endl;
      model->add_state (phase, L);
      count ++;
    }

  state.signal.connect (model, &MEAL::ComplexRVM::set_state);

  assert( count == model->get_nstate() );

  cerr << "Pulsar::ComplexRVMFit::solve using " << count << " bins" << endl;

  MEAL::LevenbergMarquardt< complex<double> > fit;
  fit.verbose = MEAL::Function::verbose;

  // MEAL::Function::verbose = true;

  float chisq = fit.init (data_x, data_y, *model);
  cerr << "initial chisq = " << chisq << endl;

  float close = 1e-3;
  unsigned iter = 1;
  unsigned not_improving = 0;
  while (not_improving < 25)
  {
    cerr << "iteration " << iter << endl;
    float nchisq = fit.iter (data_x, data_y, *model);
    cerr << "     chisq = " << nchisq << endl;

    if (nchisq < chisq)
    {
      float diffchisq = chisq - nchisq;
      chisq = nchisq;
      not_improving = 0;
      if (diffchisq/chisq < close && diffchisq > 0)
      {
	cerr << "no big diff in chisq = " << diffchisq << endl;
	break;
      }
    }
    else
      not_improving ++;

    iter ++;
  }

}
