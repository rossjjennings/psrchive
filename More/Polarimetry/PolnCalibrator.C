#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"
#include "Pulsar/CorrectionsCalibrator.h"

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Receiver.h"

#include "Pauli.h"
#include "Error.h"

#include "interpolate.h"
#include "templates.h"
#include "median_smooth.h"

#ifdef sun
#include <ieeefp.h>
#endif

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
  extension = poln_extension = archive->get<PolnCalibratorExtension>();

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
const ::MEAL::Complex2*
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
::MEAL::Complex2*
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

  if (!poln_extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::calculate_transformation",
		 "no PolnCalibratorExtension available");

  unsigned nchan = poln_extension->get_nchan();
  transformation.resize (nchan);

  for (unsigned ichan=0; ichan < nchan; ichan++)
    if ( poln_extension->get_valid(ichan) )
      transformation[ichan] = poln_extension->get_transformation(ichan);
    else
      transformation[ichan] = 0;
}

// Return the positive definite square root of a Hermitian Quaternion
template<typename T, QBasis B>
const Quaternion<T,B> mysqrt (const Quaternion<T,B>& h, float phase)
{
  T root_det = sqrt( det(h) );
  Quaternion<T,B> out;

  if (h.s0<0) {
    T scalar = -sqrt( -0.5 * (h.s0 - root_det) );
    T norm = 0.5/scalar;
    out = Quaternion<T,B> (h.s1*norm, scalar, h.s3*norm, h.s2*norm);
    // out = Quaternion<T,B> (scalar, h.get_vector()/(2*scalar));
  }
  else {
    T scalar = sqrt( 0.5 * (h.s0 + root_det) );
    out = Quaternion<T,B> (scalar, h.get_vector() * 0.5/scalar);
  }

  // cerr << phase << " in=" << h << "\n\tout=" << out << endl;
  return out;
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

  response.resize( transformation.size() );

  vector<bool> bad ( transformation.size(), false );
  unsigned ichan=0;

  for (ichan=0; ichan < response.size(); ichan++)  {

    if (transformation[ichan])  {

      // sanity check of model parameters
      unsigned nparam = transformation[ichan]->get_nparam();
      for (unsigned iparam=0; iparam < nparam; iparam++)
        if ( !finite(transformation[ichan]->get_param(iparam)) ) {

	  if (verbose)
	    cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan
		 << " " << transformation[ichan]->get_param_name(iparam)
		 << " not finite" << endl;

	  bad[ichan] = true;

        }

      double normdet = norm(det( transformation[ichan]->evaluate() ));

      if ( normdet < 1e-9 || !finite(normdet) ) {

	if (verbose)
	  cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
	    " faulty response" << endl;

	bad[ichan] = true;

      }
      else {

        response[ichan] = transformation[ichan]->evaluate();

        if (verbose)
          cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
            " response=\n" << response[ichan] << endl;

      }

    }
    else {

      if (verbose) cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan 
                        << " no transformation" << endl;

      bad[ichan] = true;

    }

    if (bad[ichan])
      response[ichan] = 0.0;

  }

  if (median_smoothing)  {

    unsigned window = unsigned (float(response.size()) * median_smoothing);
    
    if (verbose)
      cerr << "Pulsar::PolnCalibrator::build median smoothing window = "
	   << window << " channels" << endl;

    // even a 3-channel sort can zap a single channel birdie
    if (window < 3)
      window = 3;

    throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
		 "median smoothing of Jones matrices not yet implemented");

  }
  else if (interpolating) {

    for (ichan = 0; ichan < response.size(); ichan++) {

      if (!bad[ichan])
	continue;

      unsigned ifind;
	
      if (verbose)
	cerr << "Pulsar::PolnCalibrator::build interpolating ichan="
	     << ichan << endl;
      
      // find preceding good
      for (ifind=response.size()+ichan-1; ifind > ichan; ifind--)
	if (!bad[ifind%response.size()])
	  break;
      
      if (ifind == ichan)
	throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
		     "no good data");
      
      Jones<float> left = response[ifind%response.size()];
      
      // find next good
      for (ifind=ichan+1; ifind < nchan+ichan; ifind++)
	if (!bad[ifind%response.size()])
	  break;
      
      if (ifind == ichan)
	throw Error (InvalidState, "Pulsar::PolnCalibrator::build",
		     "no good data");
      
      Jones<float> right = response[ifind%response.size()];
      
      // TO-DO: It is probably more accurate to first form Mueller
      // matrices and average these as a function of frequency, in order
      // to describe any instrumental bandwidth depolarization.

      response[ichan] = float(0.5) * (left + right);
      
    }

  }

  if (nchan < response.size()) {

    // TO-DO: It is probably more accurate to first form Mueller
    // matrices and average these as a function of frequency, in order
    // to describe any instrumental bandwidth depolarization.

    unsigned factor = response.size() / nchan;
    if (nchan * factor != response.size())
      cerr << "Pulsar::PolnCalibrator::build WARNING calibrator nchan="
	   << response.size() << " mod requested nchan=" << nchan << " != 0" 
	   << endl;

    scrunch (response, factor);

    for (ichan=0; ichan<nchan; ichan++)
      response[ichan] /= factor;

  }

  complex<float> zero (0.0);

  if (nchan > response.size()) {

    if (verbose)
      cerr << "Pulsar::PolnCalibrator::build interpolating from nchan="
	   << response.size() << " to " << nchan << endl;
    
    complex<float> determinant;
    Quaternion<float, Hermitian> hermitian;
    Quaternion<float, Unitary> unitary;

    vector< Jones<float> > backup = response;

    for (ichan=0; ichan<response.size(); ichan++) {
      if (det(response[ichan]) == zero)
	continue;
      polar (determinant, hermitian, unitary, response[ichan]);
      unitary *= unitary;
      response[ichan] = determinant * (hermitian * unitary);
    }

    vector< Jones<float> > out_response (nchan);
    fft::interpolate (out_response, response);
    
    for (ichan=0; ichan<nchan; ichan++) {
      polar (determinant, hermitian, unitary, out_response[ichan]);
      unitary = sqrt(unitary);
      if (unitary[0] < 0.05)  {
        unsigned iback = (ichan * response.size()) / nchan;
        polar (determinant, hermitian, unitary, backup[iback]);
      }
      out_response[ichan] = determinant * (hermitian * unitary);
    }

    response = out_response;

  }

  Jones<double> rcvr_xform = 1.0;

  // if known, add the receiver transformation
  if (receiver) {
    rcvr_xform = receiver->get_transformation();
    if (verbose)
      cerr << "Pulsar::PolnCalibrator::build known receiver:\n"
           << rcvr_xform << endl;
  }

  for (ichan=0; ichan < nchan; ichan++) {

    if (det(response[ichan]) == zero)
      continue;

    // add the known receiver transformation
    response[ichan] *= rcvr_xform;

    // invert: the response must undo the effect of the instrument
    response[ichan] = inv (response[ichan]);

  }

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
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::add_observation",
		 "mismatch between calibrator\n\t" 
		 + calibrator->get_filename() +
                 " and\n\t" + arch->get_filename() + reason);

  if (response.size() != arch->get_nchan())
    build( arch->get_nchan() );

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::calibrate call Archive::transform" <<endl;

  if (arch->get_npol() == 4) {

    arch->transform (response);
    arch->set_poln_calibrated (true);

    if (receiver) {
      
      Receiver* rcvr = arch->get<Receiver>();
      if (!rcvr)
	throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		     "Archive has no Receiver Extension");
      
      rcvr->set_feed_corrected (true);
      
    }

  }
  else if (arch->get_npol() == 1) {

    if (Archive::verbose)
      cerr << "Pulsar::PolnCalibrator::calibrate WARNING"
	" calibrating only absolute gain" << endl;

    unsigned nsub = arch->get_nsubint ();
    unsigned nchan = arch->get_nchan ();

    for (unsigned isub=0; isub < nsub; isub++) {
      Integration* subint = arch->get_Integration (isub);
      for (unsigned ichan=0; ichan < nchan; ichan++) {

	double gain = abs(det( response[ichan] ));
	Profile* profile = subint->get_Profile (0, ichan);

	profile -> scale (gain);
	profile -> set_weight ( profile->get_weight() / gain );

      }
    }

  }
  else
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::calibrate",
		 "Archive::npol == %d not yet implemented", arch->get_npol());

  arch->set_scale (Signal::ReferenceFluxDensity);

}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::calibrate";
}



Pulsar::Calibrator::Type Pulsar::PolnCalibrator::get_type () const
{
  if (!poln_extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::get_type",
		 "no PolnCalibratorExtension available");

  return poln_extension->get_type();
}


Pulsar::CalibratorExtension*
Pulsar::PolnCalibrator::new_Extension () const
{
  return new PolnCalibratorExtension (this);
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
  const MEAL::Complex2* xform = 0;
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
