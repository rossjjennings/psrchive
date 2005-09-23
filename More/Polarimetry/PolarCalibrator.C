#include "Pulsar/PolarCalibrator.h"
#include "Pauli.h"

//! Construct from an single PolnCal Pulsar::Archive
Pulsar::PolarCalibrator::PolarCalibrator (const Archive* archive) 
  : ReferenceCalibrator (archive)
{
}

Pulsar::PolarCalibrator::~PolarCalibrator ()
{
}

//! Return the system response as determined by the Polar Transformation
::MEAL::Complex2*
Pulsar::PolarCalibrator::solve (const vector<Estimate<double> >& source,
				const vector<Estimate<double> >& sky)
{
  if ( source.size() != 4 || sky.size() != 4 )
    throw Error (InvalidParam, "Pulsar::PolarCalibrator::solve",
		 "source.size=%d or sky.size=%d != 4", 
		 source.size(), sky.size());

  if (source_set)
    throw Error (InvalidState, "Pulsar::PolarCalibrator::solve",
		 "arbitrary reference source not yet implemented");

  if (verbose)
    cerr << "Pulsar::PolarCalibrator::solve" << endl;

  // Convert the coherency vectors into Stokes parameters.  
  Stokes< Estimate<double> > stokes_source = coherency( convert (source) );
  Stokes< Estimate<double> > stokes_sky = coherency( convert (sky) );

  stokes_source *= 2.0;
  stokes_sky *= 2.0;

  Reference::To<MEAL::Polar> polar = new MEAL::Polar;

  polar->solve (stokes_source, stokes_sky);

  return polar.release();
}


Pulsar::PolarCalibrator::Info::Info (const PolnCalibrator* cal) 
  : PolnCalibrator::Info (cal)
{
}

//! Return the number of parameter classes
unsigned Pulsar::PolarCalibrator::Info::get_nclass () const
{
  return 3; 
}

//! Return the name of the specified class
const char* Pulsar::PolarCalibrator::Info::get_name (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return "Gain (C\\d0\\u)";
  case 1:
    return "Boost, sinh\\(2128)\\.m\\b\\u \\(0832)";
  case 2:
    return "\\gf\\dk\\u (rad.)";
  default:
    return "";
  }
}
  
//! Return the number of parameters in the specified class
unsigned Pulsar::PolarCalibrator::Info::get_nparam (unsigned iclass) const
{
  switch (iclass) {
  case 0:
    return 1;
  case 1:
  case 2:
    return 3;
  default:
    return 0;
  }
}


Pulsar::PolarCalibrator::Info*
Pulsar::PolarCalibrator::get_Info () const
{
  return new PolarCalibrator::Info (this);
}

Pulsar::Calibrator::Type Pulsar::PolarCalibrator::get_type () const
{
  return Polar;
}
