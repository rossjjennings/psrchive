#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"

#include "Pulsar/Receiver.h"
#include "Pulsar/Backend.h"
#include "Pulsar/FeedExtension.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

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
Pulsar::PolnCalibrator::PolnCalibrator (const Archive* archive)
{
  if (!archive)
    return;

  // store the calibrator archive
  set_calibrator (archive);

  // store the related Extension, if any
  poln_extension = archive->get<PolnCalibratorExtension>();
  if (poln_extension)
    extension = poln_extension;

  // store the Receiver Extension, if any
  receiver = archive->get<Receiver>();

  // store the Feed Extension, if any
  feed = archive->get<FeedExtension>();

  filenames.push_back( archive->get_filename() );

  built = false;
}

//! Copy constructor
Pulsar::PolnCalibrator::PolnCalibrator (const PolnCalibrator& calibrator)
{
  built = false;
}

//! Destructor
Pulsar::PolnCalibrator::~PolnCalibrator ()
{
}

//! Set the number of frequency channels in the response array
void Pulsar::PolnCalibrator::set_nchan (unsigned nchan)
{
  if (built && response.size() == nchan)
    return;

  build (nchan);
}

//! Get the number of frequency channels in the response array
unsigned Pulsar::PolnCalibrator::get_nchan () const
{
  if (!built || response.size() == 0)
    const_cast<PolnCalibrator*>(this)->build();

  return response.size ();
}

Jones<float> Pulsar::PolnCalibrator::get_response (unsigned ichan) const
{
  if (!built || response.size() == 0)
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
    setup_transformation();

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::get_transformation_nchan nchan="
         << transformation.size() << endl;

  return transformation.size();
}

//! Return the system response for the specified channel
bool Pulsar::PolnCalibrator::get_transformation_valid (unsigned ichan) const
{
  if (transformation.size() == 0)
    setup_transformation();

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
    setup_transformation();

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
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

//! Return the system response for the specified channel
MEAL::Complex2*
Pulsar::PolnCalibrator::get_transformation (unsigned ichan)
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}

void Pulsar::PolnCalibrator::setup_transformation () const
{
  if (receiver)
    Pauli::basis.set_basis( receiver->get_basis() );

  const_cast<PolnCalibrator*>(this)->calculate_transformation();
}

//! Derived classes can create and fill the transformation array
void Pulsar::PolnCalibrator::calculate_transformation ()
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::calculate_transformation" << endl;

  if (!poln_extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::calculate_transformation",
		 "no PolnCalibratorExtension available");

  unsigned nchan = poln_extension->get_nchan();
  transformation.resize (nchan);

  for (unsigned ichan=0; ichan < nchan; ichan++)
    if ( poln_extension->get_valid(ichan) )
      transformation[ichan] = 
	const_cast<MEAL::Complex2*>
	(poln_extension->get_transformation(ichan));
    else
      transformation[ichan] = 0;
}

void Pulsar::PolnCalibrator::build (unsigned nchan) try {

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build transformation size="
	 << transformation.size() << " nchan=" << nchan << endl;

  if (!built || transformation.size() == 0) {
    if (verbose > 2) cerr << "Pulsar::PolnCalibrator::build"
                         " call calculate_transformation" << endl;
    setup_transformation();
  }

  if (!nchan)
    nchan = transformation.size();

  if (verbose > 2)
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

	  if (verbose > 2)
	    cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan
		 << " " << transformation[ichan]->get_param_name(iparam)
		 << " not finite" << endl;

	  bad[ichan] = true;

        }

      double normdet = norm(det( transformation[ichan]->evaluate() ));

      if ( normdet < 1e-9 || !finite(normdet) ) {

	if (verbose > 2)
	  cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
	    " faulty response" << endl;

	bad[ichan] = true;

      }
      else {

        response[ichan] = transformation[ichan]->evaluate();

        if (verbose > 2)
          cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
            " response=\n" << response[ichan] << endl;

      }

    }
    else {

      if (verbose > 2) cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan 
                        << " no transformation" << endl;

      bad[ichan] = true;

    }

    if (bad[ichan])
      response[ichan] = 0.0;

  }

  if (median_smoothing)  {

    unsigned window = unsigned (float(response.size()) * median_smoothing);
    
    if (verbose > 2)
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
	
      if (verbose > 2)
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

    if (verbose > 2)
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

  Jones<double> feed_xform = 1.0;

  // if known, add the feed transformation
  if (feed) {
    feed_xform = feed->get_transformation();
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build known feed:\n"
           << feed_xform << endl;
  }

  Jones<double> rcvr_xform = 1.0;

  // if known, add the receiver transformation
  if (receiver) {
    rcvr_xform = receiver->get_transformation();
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build known receiver:\n"
           << rcvr_xform << endl;
  }

  for (ichan=0; ichan < nchan; ichan++) {

    if (det(response[ichan]) == zero)
      continue;

    // add the known feed transformation
    response[ichan] *= feed_xform;

    // add the known receiver transformation
    response[ichan] *= rcvr_xform;

    // invert: the response must undo the effect of the instrument
    response[ichan] = inv (response[ichan]);

  }

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build built" << endl;

  built = true;
}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::build";
}


void Pulsar::PolnCalibrator::calibration_setup (Archive* arch) try
{
  string reason;
  if (!get_calibrator()->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::add_observation",
		 "mismatch between calibrator\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + arch->get_filename() + reason);

  if (response.size() != arch->get_nchan())
    build( arch->get_nchan() );
}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::calibrate";
}

void Pulsar::PolnCalibrator::correct_backend (Archive* arch) try {

  Backend* backend = arch->get<Backend>();

  if (!backend) 
    return;

  Signal::State state = arch->get_state();
  Signal::Basis basis = arch->get_basis();
  Signal::Hand hand = backend->get_hand();
  Signal::Argument argument = backend->get_argument();

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::correct_backend basis=" << basis
	 << " hand=" << hand << " phase=" << argument << endl;

  unsigned npol = arch->get_npol();

  if (npol < 2)
    return;

  bool swap01 = false;
  bool flip[4] = { false, false, false, false };

  if (npol == 4) {
    if (argument == Signal::Conjugate)
      if (state == Signal::Stokes && basis == Signal::Circular)
	flip[2] = !flip[2];   // Flip Stokes U
      else
	flip[3] = !flip[3];   // Flip Stokes V or Im[AB]

    if (hand == Signal::Left) {
      if (state == Signal::Stokes && basis == Signal::Circular)
	flip[2] = !flip[2];   // Flip Stokes U and ...
      else if (state == Signal::Stokes && basis == Signal::Linear)
	flip[1] = !flip[1];   // Flip Stokes Q and ...
      flip[3] = !flip[3];     // Flip Stokes V or Im[AB]
    }
  }

  // If state == Coherence or PPQQ ...
  if (hand == Signal::Left && state != Signal::Stokes)
    swap01 = true;

  bool flip_something = false;
  for (unsigned ipol=0; ipol < npol; ipol++)
    if (flip[ipol]) {
      if (verbose > 2)
	cerr << "Pulsar::PolnCalibrator::correct_backend flip ipol=" << ipol
	     << endl;
      flip_something = true;
    }

  if (swap01 && verbose > 2)
    cerr << "Pulsar::PolnCalibrator::correct_backend swap 0 and 1" << endl;

  if (flip_something || swap01) {
    for (unsigned isub=0; isub < arch->get_nsubint(); isub++) {
      Integration* integration = arch->get_Integration(isub);
      for (unsigned ichan=0; ichan < arch->get_nchan(); ichan++) {
	
	for (unsigned ipol=0; ipol < npol; ipol++)
	  if (flip[ipol])
	    integration->get_Profile(ipol, ichan)->scale(-1);
	
	if (swap01 == -1)
	  integration->expert()->swap_profiles(0, ichan, 1, ichan);
	
      }
    }
  }

  backend->set_argument (Signal::Conventional);
  backend->set_hand (Signal::Right);

}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::correct_backend";
}

/*! Upon completion, the flux of the archive will be normalized with
  respect to the flux of the calibrator, such that a FluxCalibrator
  simply scales the archive by the calibrator flux. */
void Pulsar::PolnCalibrator::calibrate (Archive* arch) try {

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::calibrate" << endl;

  calibration_setup (arch);

  if (arch->get_npol() == 4) {

    correct_backend (arch);

    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::calibrate Archive::transform" <<endl;

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

    if (Archive::verbose > 2)
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
    if (verbose > 2) cerr << "Pulsar::PolnCalibrator::Info::get_param"
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

