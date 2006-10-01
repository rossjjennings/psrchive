/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "Pulsar/Calibrator.h"
#include "Pulsar/CalibratorExtension.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Config.h"

using namespace std;

unsigned Pulsar::Calibrator::verbose
= Pulsar::config.get<bool> ("Calibrator::verbose", 0);

/*! The size of the window used during median filtering is given by
   the number of frequency channels, nchan, multiplied by
   median_smoothing.  If set to zero, no smoothing is performed.  A
   sensible value is around 0.05. */
float Pulsar::Calibrator::median_smoothing
= Pulsar::config.get<float> ("Calibrator::median_smoothing", 0.0);

/*! The maximum number of channels over which a linear interpolation will be
  performed */
float Pulsar::Calibrator::interpolating 
= Pulsar::config.get<float> ("Calibrator::interpolating", 0.0);

/*! Although a very sensible constraint, this option is disabled by default */
float Pulsar::Calibrator::physical_det_threshold
= Pulsar::config.get<float>("Calibrator::physical_det_threshold", 0.0);


Pulsar::Calibrator::Calibrator ()
{
}

Pulsar::Calibrator::~Calibrator ()
{
}


//! Provide derived classes with access to the calibrator
bool Pulsar::Calibrator::has_calibrator () const
{
  return calibrator;
}

//! Provide derived classes with access to the calibrator
const Pulsar::Archive* Pulsar::Calibrator::get_calibrator () const
{
  return calibrator;
}

//! Provide derived classes with access to the calibrator
void Pulsar::Calibrator::set_calibrator (const Archive* archive)
{
  calibrator = archive;
}

//! Provide access to Integration::transform
void Pulsar::Calibrator::calibrate (Integration* integration,
				    const vector< Jones<float> >& response)
{
  integration->transform (response);
}

//! Provide access to Integration::transform
void Pulsar::Calibrator::calibrate (Integration* integration,
				    const Jones<float>& response)
{
  integration->transform (response);
}

//! Return a string containing the file information
string Pulsar::Calibrator::get_filenames () const
{
  string names;

  if (filenames.size())
    names = filenames[0];

  for (unsigned i=1; i<filenames.size(); i++)
    names += "\n\t" + filenames[i];

  return names;

}

//! Return a const reference to the calibrator archive
const Pulsar::Archive* Pulsar::Calibrator::get_Archive () const
{
  if (!calibrator)
    throw Error (InvalidState, "Pulsar::Calibrator::get_Archive",
		 "no calibrator Archive");

  return calibrator; 
}

MJD Pulsar::Calibrator::get_epoch () const
{
  if (extension)
    return extension->get_epoch ();

  return 0.5 * (calibrator->start_time() + calibrator->end_time());
}

Pulsar::Archive*
Pulsar::Calibrator::new_solution (const string& archive_class) const
{
  if (verbose > 2) cerr << "Pulsar::PolnCalibrator::new_solution"
		     " create PolnCalibratorExtension" << endl;

  Reference::To<CalibratorExtension> ext = new_Extension ();

  if (verbose > 2) cerr << "Pulsar::PolnCalibrator::new_solution"
		     " create " << archive_class << endl;
  
  Reference::To<Archive> output = Pulsar::Archive::new_Archive (archive_class);
  output -> copy (*calibrator);
  output -> resize (0);
  output -> set_type (Signal::Calibrator);
  output -> add_extension (ext);

  return output.release();
}

