/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// #define _DEBUG 1

#include "Pulsar/Calibrator.h"
#include "Pulsar/CalibratorExtension.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Archive.h"

using namespace std;

#ifdef _DEBUG
static int runme () { cerr << "Calibrator: init" << endl; return 0; }
static int test = runme ();
#endif

Pulsar::Option<unsigned> 
Pulsar::Calibrator::verbose ("Calibrator::verbose", 0);

/*! The size of the window used during median filtering is given by
   the number of frequency channels, nchan, multiplied by
   median_smoothing.  If set to zero, no smoothing is performed.  A
   sensible value is around 0.05. */
Pulsar::Option<float>
Pulsar::Calibrator::median_smoothing ("Calibrator::median_smoothing", 0.0);

/*! The maximum number of channels over which a linear interpolation will be
  performed */
Pulsar::Option<float>
Pulsar::Calibrator::interpolating ("Calibrator::interpolating", 0.0);

/*! Although a very sensible constraint, this option is disabled by default */
Pulsar::Option<float>
Pulsar::Calibrator::det_threshold ("Calibrator::det_threshold", 0.0);


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

//! Calibrator::Type output operator
std::ostream& operator << (std::ostream& os, Pulsar::Calibrator::Type type)
{
  return os << Pulsar::Calibrator::Type2str (type);
}

//! Calibrator::Type input operator
std::istream& operator >> (std::istream& is, Pulsar::Calibrator::Type& type)
{
  std::streampos pos = is.tellg();
  std::string ss;

  is >> ss;
  type = Pulsar::Calibrator::str2Type (ss.c_str());

  if (type == (Pulsar::Calibrator::Type) -1) {
    is.setstate(std::istream::failbit);
    is.seekg(pos);
  }

  return is;
}
