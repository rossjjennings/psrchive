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




SingleAxisCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

string SingleAxisCalibrator::Info::get_title () const
{
  return "Polar Decomposition of Complex Gains";
}

//! Return the number of parameter classes
unsigned SingleAxisCalibrator::Info::get_nclass () const
{
  return 3; 
}

//! Return the name of the specified class
string SingleAxisCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "\\fiG\\fr (\\fic\\fr\\d0\\u)";
  case 1:
    return "\\gg (%)";
  case 2:
    return "\\gf (\\(2729))";
  default:
    return "";
  }
} 
  
//! Return the number of parameters in the specified class
unsigned SingleAxisCalibrator::Info::get_nparam (unsigned iclass) const
{
  if (iclass < 3)
    return 1;
  return 0;
}

Estimate<float>
SingleAxisCalibrator::Info::get_param (unsigned ichan, 
					       unsigned iclass,
					       unsigned iparam) const
{
  if (iclass == 0)
    return PolnCalibrator::Info::get_param (ichan, iclass, iparam);

  /*
    See Equation (25) of van Straten (2006) and the text that follows it.

    The SingleAxis model is parameterized by beta and this object provides gamma.
  */
  
  if (iclass == 1)
    return 100.0 * 
      ( exp( 2*PolnCalibrator::Info::get_param (ichan, iclass, iparam) ) - 1 );

  // iclass == 2
  return 180.0/M_PI * PolnCalibrator::Info::get_param (ichan, iclass, iparam);
}



SingleAxisCalibrator::Info* 
SingleAxisCalibrator::get_Info () const
{
  return new SingleAxisCalibrator::Info (this);
}

