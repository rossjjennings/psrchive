/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/BackendEstimate.h"
#include "Pulsar/SingleAxis.h"
#include "MEAL/Polar.h"

using namespace std;

static bool verbose = false;

void Calibration::BackendEstimate::integrate (const MEAL::Complex2* xform)
{
  const MEAL::Polar* polar_solution;
  polar_solution = dynamic_cast<const MEAL::Polar*>( xform );

  if (polar_solution)
  {
    if (verbose)
      cerr << "BackendEstimate::integrate Polar" << endl;
    
    polar_estimate.integrate( polar_solution );
    return;
  }

  const Calibration::SingleAxis* sa;
  sa = dynamic_cast<const Calibration::SingleAxis*>( xform );

  if (sa)
  {
    if (verbose)
      cerr << "BackendEstimate::integrate SingleAxis" << endl;
    
    estimate.integrate( sa );

    return;
  }

  const MEAL::ProductRule<MEAL::Complex2>* product;
  product = dynamic_cast<const MEAL::ProductRule<MEAL::Complex2>*>( xform );
  if (product)
  {
    if (verbose)
      cerr << "BackendEstimate::integrate ProductRule" << endl;
    
    for (unsigned imodel=0; imodel<product->get_nmodel(); imodel++)
      integrate( product->get_model(imodel) );

    return;
  }
}

void Calibration::BackendEstimate::update ()
{
  SingleAxis* single = dynamic_cast<SingleAxis*>( backend.get() );
  if (single)
  {
    estimate.update (single);
    return;
  }

  MEAL::Polar* polar = dynamic_cast<MEAL::Polar*>( backend.get() );
  if (polar)
  {
    polar_estimate.update (polar);
    return;
  }
}
