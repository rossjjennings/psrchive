#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"

#include "Pauli.h"
#include "Error.h"

#include "interpolate.h"
#include "smooth.h"

//! Constructor
Pulsar::PolnCalibrator::PolnCalibrator ()
{
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

//! Get the number of frequency channels in the transformation array
unsigned Pulsar::PolnCalibrator::get_Transformation_nchan () const
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  return transformation.size();
}

//! Return the system response for the specified channel
bool Pulsar::PolnCalibrator::get_Transformation_valid (unsigned ichan) const
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_Transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Return the system response for the specified channel
const ::Calibration::Transformation*
Pulsar::PolnCalibrator::get_Transformation (unsigned ichan) const
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_Transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}



void Pulsar::PolnCalibrator::build (unsigned nchan)
{
  if (transformation.size() == 0)
    const_cast<PolnCalibrator*>(this)->calculate_transformation();

  if (!nchan)
    nchan = transformation.size();

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::build nchan=" << nchan 
	 << " transformation.size=" << transformation.size() << endl;

  if (!nchan || nchan == response.size())
    return;

  response.resize( transformation.size() );

  for (unsigned ichan=0; ichan < response.size(); ichan++)
    if (transformation[ichan])
      response[ichan] = inv( transformation[ichan]->evaluate() );
    else
      response[ichan] = Jones<float>::identity();

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





/*! Upon completion, the flux of the archive will be normalized with
  respect to the flux of the calibrator, such that a FluxCalibrator
  simply scales the archive by the calibrator flux. */
void Pulsar::PolnCalibrator::calibrate (Archive* arch)
{
  cerr << "Pulsar::PolnCalibrator::calibrate" << endl;

  if (!calibrator)
    throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		 "no PolnCal Archive");

  string reason;
  if (!calibrator->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::FluxCalibrator", "Pulsar::Archive='"
		 + calibrator->get_filename() + "'\ndoes not mix with '"
		 + arch->get_filename() + reason);

  if (response.size() != arch->get_nchan())
    build( arch->get_nchan() );

  arch->transform (response);
}

//! Constructor
Pulsar::PolnCalibrator::Info::Info (const PolnCalibrator* cal)
{
  nparam = 0;

  if (!cal)
    return;

  calibrator = cal;
  
  unsigned nchan = cal->get_Transformation_nchan ();

  // find the first valid transformation
  const Calibration::Transformation* xform = 0;
  for (unsigned ichan = 0; ichan < nchan; ichan++) {
    xform = cal->get_Transformation (ichan);
    if (xform)
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
  if (! calibrator->get_Transformation_valid(ichan) )
    return 0;

  unsigned offset = 0;
  for (unsigned jclass=1; jclass<=iclass; jclass++)
    offset += get_nparam (jclass-1);

  return calibrator->get_Transformation(ichan)->get_Estimate(iparam+offset);
}

Pulsar::Calibrator::Info* Pulsar::PolnCalibrator::get_Info () const
{
  return new PolnCalibrator::Info (this);
}
