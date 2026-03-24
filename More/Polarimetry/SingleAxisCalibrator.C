/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/SingleAxisCalibrator.h"
#include "Pulsar/CalibratorType.h"

#include "Pauli.h"
#include "Estimate.h"

using namespace std;
using namespace Pulsar;

//! Construct from an single PolnCal Archive
SingleAxisCalibrator::SingleAxisCalibrator (const Archive* archive) 
  : ReferenceCalibrator (archive)
{
  type = Calibrator::Type::factory (this);
}

//! Copy constructor
SingleAxisCalibrator::SingleAxisCalibrator (const SingleAxisCalibrator& other)
  : ReferenceCalibrator (other)  
{
  type = Calibrator::Type::factory (this);
}

//! Clone operator
SingleAxisCalibrator* SingleAxisCalibrator::clone () const
{
  return new SingleAxisCalibrator (*this);
}

SingleAxisCalibrator::~SingleAxisCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Return the system response as determined by the SingleAxis
::MEAL::Complex2*
SingleAxisCalibrator::solve (const vector<Estimate<double> >& source,
				     const vector<Estimate<double> >& sky)
{
  Reference::To<Calibration::SingleAxis> model = new Calibration::SingleAxis;

  if ( !source_set || source.size() != 4 )
  {
    if (verbose > 2)
      cerr << "SingleAxisCalibrator::solve" << endl;
    model->solve (source);
    return model.release();
  }

  if (verbose > 2)
    cerr << "SingleAxisCalibrator::solve reference source=" 
         << reference_source << endl;

  // Convert the coherency vectors into Stokes parameters.
  Stokes< Estimate<double> > stokes_source = coherency( convert (source) );

  if (!solver)
    solver = new Calibration::SingleAxisSolver;

  solver->set_input (reference_source);
  solver->set_output (stokes_source);
  solver->solve (model);

  return model.release();
}


