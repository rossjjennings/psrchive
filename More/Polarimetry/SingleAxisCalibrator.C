#include "Pulsar/SingleAxisCalibrator.h"
#include "Pauli.h"
#include "Estimate.h"

//! Construct from an single PolnCal Pulsar::Archive
Pulsar::SingleAxisCalibrator::SingleAxisCalibrator (const Archive* archive) 
  : ReferenceCalibrator (archive)
{
}

Pulsar::SingleAxisCalibrator::~SingleAxisCalibrator ()
{
  // destructors must be defined in .C file so that the Reference::To
  // desctructor can delete forward declared objects
}

//! Return the system response as determined by the SingleAxis
::Calibration::Complex2*
Pulsar::SingleAxisCalibrator::solve (const vector<Estimate<double> >& source,
				     const vector<Estimate<double> >& sky)
{
  Reference::To<Calibration::SingleAxis> model = new Calibration::SingleAxis;

  if (!source_set) {
    if (verbose)
      cerr << "Pulsar::SingleAxisCalibrator::solve" << endl;
    model->solve (source);
    return model.release();
  }

  if ( source.size() != 4 )
    throw Error (InvalidParam, "Pulsar::SingleAxisCalibrator::solve",
                 "source.size=%d != 4", source.size());

  if (verbose)
    cerr << "Pulsar::SingleAxisCalibrator::solve reference source=" 
         << reference_source << endl;

  // Convert the coherency vectors into Stokes parameters.
  Stokes< Estimate<double> > stokes_source = coherency( convert (source) );
  stokes_source *= 2.0;

  if (!solver)
    solver = new Calibration::SingleAxisSolver;

  solver->set_input (reference_source);
  solver->set_output (stokes_source);
  solver->solve (model);

  // model->solve( reference_source, stokes_source );

  return model.release();
}




Pulsar::SingleAxisCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

//! Return the number of parameter classes
unsigned Pulsar::SingleAxisCalibrator::Info::get_nclass () const
{
  return 3; 
}

//! Return the name of the specified class
const char* Pulsar::SingleAxisCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "\\fiG\\fn (\\fic\\fn\\d0\\u)";
  case 1:
    return "\\gg (\\x10)";
  case 2:
    return "\\gf (deg.)";
  default:
    return "";
  }
} 
  
//! Return the number of parameters in the specified class
unsigned Pulsar::SingleAxisCalibrator::Info::get_nparam (unsigned iclass) const
{
  if (iclass < 3)
    return 1;
  return 0;
}

//! Return the scale of parameters in the specified class
float Pulsar::SingleAxisCalibrator::Info::get_scale (unsigned iclass) const
{
  if (iclass == 1)
    return 10.0;

  if (iclass == 2)
    return 180.0 / M_PI;
  
  return 1.0;
}

Pulsar::SingleAxisCalibrator::Info* 
Pulsar::SingleAxisCalibrator::get_Info () const
{
  return new SingleAxisCalibrator::Info (this);
}

Pulsar::Calibrator::Type Pulsar::SingleAxisCalibrator::get_type () const
{
  return SingleAxis;
}

