#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CorrectionsCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Receiver.h"

#include "Pauli.h"
#include "Error.h"

#include "interpolate.h"
#include "smooth.h"

/*! 
  If a Pulsar::Archive is provided, and if it contains a
  PolnCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::PolnCalibrator::PolnCalibrator (Archive* archive)
{
  if (!archive)
    return;

  // store the calibrator archive
  calibrator = archive;

  // store the related Extension, if any
  extension = archive->get<PolnCalibratorExtension>();

  // store the Receiver Extension, if any
  receiver = archive->get<Receiver>();

  filenames.push_back( archive->get_filename() );

}

//! Copy constructor
Pulsar::PolnCalibrator::PolnCalibrator (const PolnCalibrator& calibrator)
{
}

//! Destructor
Pulsar::PolnCalibrator::~PolnCalibrator ()
{
}

//! Set the number of frequency channels in the response array
void Pulsar::PolnCalibrator::set_nchan (unsigned nchan)
{
  if (response.size() == nchan)
    return;

  build (nchan);
}

//! Get the number of frequency channels in the response array
unsigned Pulsar::PolnCalibrator::get_nchan () const
{
  if (response.size() == 0)
    const_cast<PolnCalibrator*>(this)->build();

  return response.size ();
}

Jones<float> Pulsar::PolnCalibrator::get_response (unsigned ichan) const
{
  if (response.size() == 0)
    const_cast<PolnCalibrator*>(this)->build();

  if (ichan >= response.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_response",
		 "ichan=%d >= nchan=%d", ichan, response.size());

  return response[ichan];
}

//! Return true if the Receiver is set
bool Pulsar::PolnCalibrator::has_Receiver () const
{
  return receiver;
}

//! Return the Receiver
const Pulsar::Receiver* Pulsar::PolnCalibrator::get_Receiver () const
{
  return receiver;
}

//! Get the number of frequency channels in the transformation array
unsigned Pulsar::PolnCalibrator::get_transformation_nchan () const
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::get_transformation_nchan nchan="
         << transformation.size() << endl;

  return transformation.size();
}

//! Return the system response for the specified channel
bool Pulsar::PolnCalibrator::get_transformation_valid (unsigned ichan) const
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam,
		 "Pulsar::PolnCalibrator::get_transformation_valid",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Return the system response for the specified channel
void Pulsar::PolnCalibrator::set_transformation_invalid (unsigned ichan)
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam,
		 "Pulsar::PolnCalibrator::set_transformation_invalid",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  transformation[ichan] = 0;

}

//! Return the system response for the specified channel
const ::Calibration::Complex2*
Pulsar::PolnCalibrator::get_transformation (unsigned ichan) const
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Return the system response for the specified channel
::Calibration::Complex2*
Pulsar::PolnCalibrator::get_transformation (unsigned ichan)
{
  if (transformation.size() == 0)
    calculate_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Derived classes can create and fill the transformation array
void Pulsar::PolnCalibrator::calculate_transformation ()
{
  if (verbose)
    cerr << "Pulsar::PolnCalibrator::calculate_transformation" << endl;

  if (!extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::calculate_transformation",
		 "no PolnCalibratorExtension available");

  unsigned nchan = extension->get_nchan();
  transformation.resize (nchan);

  for (unsigned ichan=0; ichan < nchan; ichan++)
    if ( extension->get_valid(ichan) )
      transformation[ichan] = extension->get_transformation(ichan);
    else
      transformation[ichan] = 0;
}

void Pulsar::PolnCalibrator::build (unsigned nchan) try {

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::build transformation size="
	 << transformation.size() << " nchan=" << nchan << endl;

  if (transformation.size() == 0) {
    if (verbose) cerr << "Pulsar::PolnCalibrator::build"
                         " call calculate_transformation" << endl;
    const_cast<PolnCalibrator*>(this)->calculate_transformation();
  }

  if (!nchan)
    nchan = transformation.size();

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::build nchan=" << nchan 
	 << " transformation.size=" << transformation.size() << endl;

  if (!nchan || nchan == response.size())
    return;

  response.resize( transformation.size() );

  for (unsigned ichan=0; ichan < response.size(); ichan++)  {

    if (transformation[ichan])  {

      // sanity check of model parameters
      unsigned nparam = transformation[ichan]->get_nparam();
      for (unsigned iparam=0; iparam < nparam; iparam++)
        if ( !finite(transformation[ichan]->get_param(iparam)) ) {

	  if (verbose)
	    cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan
		 << " " << transformation[ichan]->get_param_name(iparam)
		 << " not finite" << endl;
          response[ichan] = Jones<float>::identity();

        }

      double normdet = norm(det( transformation[ichan]->evaluate() ));

      if ( normdet < 1e-9 || !finite(normdet) ) {

	if (verbose)
	  cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
	    " faulty response" << endl;

	response[ichan] = Jones<float>::identity();

      }
      else {

        response[ichan] = inv( transformation[ichan]->evaluate() );

        if (verbose)
          cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
            " response=\n" << response[ichan] << endl;

      }

    }
    else {

      if (verbose) cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan 
                        << " no transformation" << endl;

      response[ichan] = Jones<float>::identity();

    }

    if (receiver)
      response[ichan] *= receiver->get_correction();

  }

  if (median_smoothing)  {

    unsigned window = unsigned (float(response.size()) * median_smoothing);
    
    if (verbose)
      cerr << "Pulsar::PolnCalibrator::build median smoothing window = "
	   << window << " channels" << endl;

    // even a 3-window sort can zap a single channel birdie
    if (window < 3)
      window = 3;

    throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
		 "median smoothing of Jones matrices not yet implemented");

  }

  if (response.size() == nchan)
    return;
 
  throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
	       "interpolating/averaging Jones matrices not yet implemented");
}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::build";
}

/*! Upon completion, the flux of the archive will be normalized with
  respect to the flux of the calibrator, such that a FluxCalibrator
  simply scales the archive by the calibrator flux. */
void Pulsar::PolnCalibrator::calibrate (Archive* arch) try {

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::calibrate" << endl;

  if (!calibrator)
    throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		 "no PolnCal Archive");

  string reason;
  if (!calibrator->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + calibrator->get_filename() + "'\ndoes not match '"
		 + arch->get_filename() + reason);

  if (response.size() != arch->get_nchan())
    build( arch->get_nchan() );

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::calibrate call Archive::transform" <<endl;

  arch->transform (response);

  arch->set_poln_calibrated (true);
  arch->set_scale (Signal::ReferenceFluxDensity);

  if (receiver) {

    Receiver* rcvr = arch->get<Receiver>();
    if (!rcvr)
      throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		   "Archive has no Receiver Extension");

    rcvr->set_feed_corrected (true);

  }

}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::calibrate";
}



Pulsar::Calibrator::Type Pulsar::PolnCalibrator::get_type () const
{
  if (!extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::get_type",
		 "no PolnCalibratorExtension available");

  return extension->get_type();
}

MJD Pulsar::PolnCalibrator::get_epoch () const
{
  if (extension)
    return extension->get_epoch ();

  return 0.5 * (calibrator->start_time() + calibrator->end_time());
}

Pulsar::Archive*
Pulsar::PolnCalibrator::get_solution (const string& archive_class,
				      string filename_extension) const
{
  if (verbose) cerr << "Pulsar::PolnCalibrator::get_solution"
		 " create PolnCalibratorExtension" << endl;

  Reference::To<PolnCalibratorExtension> ext;
  ext = new PolnCalibratorExtension (this);

  if (verbose) cerr << "Pulsar::PolnCalibrator::get_solution"
		 " create " << archive_class << endl;
  
  Reference::To<Archive> output = Pulsar::Archive::new_Archive (archive_class);
  output -> copy (*calibrator);
  output -> resize (0);
  output -> set_type (Signal::Calibrator);
  output -> add_extension (ext);
  
  string filename = calibrator->get_filename();
  int index = filename.find_last_of(".", 0);
  filename = filename.substr(0, index) + filename_extension;

  output -> set_filename (filename);

  return output.release();
}


//! Constructor
Pulsar::PolnCalibrator::Info::Info (const PolnCalibrator* cal)
{
  nparam = 0;

  if (!cal)
    return;

  calibrator = cal;
  
  unsigned nchan = cal->get_transformation_nchan ();

  // find the first valid transformation
  const Calibration::Complex2* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++)
    if ( cal->get_transformation_valid (ichan) ) {
      xform = cal->get_transformation (ichan);
      break;
    }

  if (!xform)
    return;

  nparam = xform->get_nparam();
}
      
/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::PolnCalibrator::Info::get_nclass () const
{
  return nparam;
}

/*! The name of each parameter is unknown */
const char* Pulsar::PolnCalibrator::Info::get_name (unsigned iclass) const
{
  return "unknown";
}

/*! Each parameter of the Transformation is treated as a separate class. */
unsigned Pulsar::PolnCalibrator::Info::get_nparam (unsigned iclass) const
{
  return 1;
}
      
//! Return the estimate of the specified parameter
Estimate<float> Pulsar::PolnCalibrator::Info::get_param (unsigned ichan, 
							 unsigned iclass,
							 unsigned iparam) const
{
  if (! calibrator->get_transformation_valid(ichan) ) {
    if (verbose) cerr << "Pulsar::PolnCalibrator::Info::get_param"
		   " invalid ichan=" << ichan << endl;
    return 0;
  }

  unsigned offset = 0;
  for (unsigned jclass=1; jclass<=iclass; jclass++)
    offset += get_nparam (jclass-1);

  return calibrator->get_transformation(ichan)->get_Estimate(iparam+offset);
}


//! Return the colour index
int Pulsar::PolnCalibrator::Info::get_colour_index (unsigned iclass,
						    unsigned iparam) const
{
  unsigned colour_offset = 1;
  if (get_nparam (iclass) == 3)
    colour_offset = 2;

  return colour_offset + iparam;
}

//! Return the graph marker
int Pulsar::PolnCalibrator::Info::get_graph_marker (unsigned iclass, 
						    unsigned iparam) const
{
  if (iparam == 0 || iparam > 3)
    return -1;

  iparam --;

  int nice_markers[3] = { 2, 5, 4 };

  return nice_markers[iparam];
}


Pulsar::PolnCalibrator::Info* Pulsar::PolnCalibrator::get_Info () const
{
  return PolnCalibrator::Info::create (this);
}
