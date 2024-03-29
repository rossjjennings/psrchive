/***************************************************************************
 *
 *   Copyright (C) 2003 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Pulsar/PolnCalibrator.h"
#include "Pulsar/PolnCalibratorExtension.h"

#ifdef HAVE_SPLINTER
#include "Pulsar/CalibrationInterpolator.h"
#endif

#include "Pulsar/CalibrationInterpolatorExtension.h"

#include "Pulsar/Receiver.h"
#include "Pulsar/BasisCorrection.h"
#include "Pulsar/BackendCorrection.h"
#include "Pulsar/FeedExtension.h"
#include "Pulsar/ProcHistory.h"

#include "Pulsar/Archive.h"
#include "Pulsar/IntegrationExpert.h"
#include "Pulsar/Profile.h"

#include "Pauli.h"
#include "Error.h"

#include "interpolate.h"
#include "templates.h"
#include "median_smooth.h"
#include "strutil.h"
#include "debug.h"
#include "myfinite.h"

#ifdef sun
#include <ieeefp.h>
#endif

using namespace std;

Pulsar::Option<double>
Pulsar::PolnCalibrator::minimum_determinant
(
 "PolnCalibrator::minimum_determinant", 1e-9,

 "Minimum allowable determinant of a Jones matrix",

 "To avoid unstable inversion of matrices with a determinant close to zero, \n"
 "any frequency channel with a polarimetric response described by a Jones \n"
 "matrix with a determinant smaller than this value will be flagged as invalid."
);


void Pulsar::PolnCalibrator::init ()
{
  do_backend_correction = true;
  built = false;
  observation_nchan = 0;

  // by default construct from all sub-integrations
  subint.set_integrate (true);
}

/*!
  If a Pulsar::Archive is provided, and if it contains a
  PolnCalibratorExtension, then the constructed instance can be
  used to calibrate other Pulsar::Archive instances.
*/
Pulsar::PolnCalibrator::PolnCalibrator (const Archive* archive)
{
  init ();

  if (!archive)
    return;

  // store the calibrator archive
  set_calibrator (archive);

  if (archive->get<CalibrationInterpolatorExtension> ())
  {
#ifdef HAVE_SPLINTER
    DEBUG("PolnCalibrator ctor set variation");
    set_variation( new CalibrationInterpolator (this) );
#else
    throw Error (InvalidState, "PolnCalibrator ctor",
		 "Archive has CalibrationInterpolatorExtension\n\t"
		 "but SPLINTER library not available to interpret it");
#endif
  }

  // store the related Extension, if any
  poln_extension = archive->get<PolnCalibratorExtension>();
  if (poln_extension)
    extension = poln_extension;

  filenames.push_back( archive->get_filename() );
}

void Pulsar::PolnCalibrator::set_Receiver (const Archive* archive)
{
  if (!archive)
  {
    receiver = 0;
    receiver_basis_filename = "";
  }
  else
  {
    receiver = archive->get<Receiver>();
    receiver_basis_filename = archive->get_filename();
    if (verbose)
      cerr << "PolnCalibrator::set_Receiver filename=" 
           << archive->get_filename() << endl;
  }
}

std::string Pulsar::PolnCalibrator::get_receiver_basis_filename () const
{
  return receiver_basis_filename;
}

void Pulsar::PolnCalibrator::set_calibrator (const Archive* archive)
{
  // store the Receiver Extension, if any
  if (!has_Receiver())
    set_Receiver (archive);

  // store the Feed Extension, if any
  feed = archive->get<FeedExtension>();

  built = false;
  observation_nchan = 0;
  transformation_resize(0);

  Calibrator::set_calibrator (archive);
}

void Pulsar::PolnCalibrator::set_subint (const Index& isub)
{
  subint = isub;
  set_calibrator ( get_Archive() );
}

//! Copy constructor
Pulsar::PolnCalibrator::PolnCalibrator (const PolnCalibrator& calibrator)
{
  init ();
}

//! Destructor
Pulsar::PolnCalibrator::~PolnCalibrator ()
{
}

//! Set the number of frequency channels in the response array
void Pulsar::PolnCalibrator::set_response_nchan (unsigned nchan)
{
  if (built && response.size() == nchan)
    return;

  build (nchan);
}

//! Get the number of frequency channels in the response array
unsigned Pulsar::PolnCalibrator::get_response_nchan () const
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

/*!  

  Some calibrators (e.g. HybridCalibrator) may be capable of shrinking
  the size of the transformation array to match the number of
  frequency channels in the observation.  If this is done, and the
  next observation to be calibrated has more channels than the first,
  then the transformation array should be recomputed to match the new
  frequency resolution.

  However, to date, the decision to recompute the transformation array
  has been based on it having zero size.  It is not possible to
  compare tranformation.size() with get_nchan(), because
  PolnCalibrator::get_nchan makes the same comparison and returns
  transformation.size().

  Rather than add a new method that must be implemented by all of the
  derived classes, this new method gives shrink-capable classes the
  opportunity to communicate this feature.

  By default, the method returns zero.  Otherwise, it should return
  the maximum possible size of the transformation array that can be
  supported.

*/
unsigned Pulsar::PolnCalibrator::get_maximum_nchan () const
{
  return 0;
}


//! Get the number of frequency channels in the calibrator
unsigned Pulsar::PolnCalibrator::get_nchan () const
{
  return get_nchan (true);
}

unsigned Pulsar::PolnCalibrator::get_nchan (bool build_if_needed) const try
{
  if (build_if_needed && transformation.size() == 0)
    setup_transformation();

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::get_nchan nchan="
         << transformation.size() << endl;

  return transformation.size();
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::get_nchan";
}

//! Return the system response for the specified channel
bool Pulsar::PolnCalibrator::get_transformation_valid (unsigned ichan) const try
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam,
		 "Pulsar::PolnCalibrator::get_transformation_valid",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::get_transformation_valid";
}

//! Return the system response for the specified channel
void Pulsar::PolnCalibrator::set_transformation_invalid (unsigned ichan, const string& reason) try
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam,
		 "Pulsar::PolnCalibrator::set_transformation_invalid",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  if (verbose)
    cerr << "Pulsar::PolnCalibrator::set_transformation_invalid ichan=" << ichan << " reason=" << reason << endl;

  transformation[ichan] = 0;
  invalid_reason[ichan] = reason;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::set_transformation_invalid";
}

const std::string& Pulsar::PolnCalibrator::get_transformation_invalid_reason (unsigned ichan) const
{
  if (ichan >= invalid_reason.size())
    throw Error (InvalidParam,
                 "Pulsar::PolnCalibrator::get_transformation_invalid_reason",
                 "ichan=%d >= nchan=%d", ichan, invalid_reason.size());

  return invalid_reason[ichan];
}

//! Return the system response for the specified channel
const ::MEAL::Complex2*
Pulsar::PolnCalibrator::get_transformation (unsigned ichan) const try
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::get_transformation const";
}

//! Return the system response for the specified channel
MEAL::Complex2*
Pulsar::PolnCalibrator::get_transformation (unsigned ichan) try
{
  if (transformation.size() == 0)
    setup_transformation();

  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
		 "ichan=%d >= nchan=%d", ichan, transformation.size());

  return transformation[ichan];
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::get_transformation";
}

void Pulsar::PolnCalibrator::set_transformation (unsigned ichan, MEAL::Complex2* xform)
{
  if (ichan >= transformation.size())
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::get_transformation",
                 "ichan=%d >= nchan=%d", ichan, transformation.size());

  transformation[ichan] = xform;
  invalid_reason[ichan] = "";
}

//! Return true if parameter covariances are stored
bool Pulsar::PolnCalibrator::has_covariance () const try
{
  if (transformation.size() == 0)
    setup_transformation();

  return (covariance.size() == get_nchan());
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::has_covariance";
}

//! Return the covariance matrix vector for the specified channel
void Pulsar::PolnCalibrator::get_covariance (unsigned ichan,
					     std::vector<double>& c) const
{
  assert (ichan < covariance.size());
  c = covariance[ichan];
}

//! Return true if least squares minimization solvers are available
bool Pulsar::PolnCalibrator::has_solver () const
{
  return poln_extension && poln_extension->get_has_solver();
}

class TmpSolver : public MEAL::LeastSquares
{
public:
  TmpSolver (float _chisq, unsigned _nfree, unsigned _nfit)
  {
    best_chisq = _chisq; nfree = _nfree; nparam_infit = _nfit; solved = true;
  }
  string get_name () const { return "TmpSolver"; }
};

//! Return the transformation for the specified channel
const MEAL::LeastSquares*
Pulsar::PolnCalibrator::get_solver (unsigned ichan) const
{
  if (!has_solver())
    throw Error (InvalidState, "Pulsar::PolnCalibrator::get_solver",
		 "no solver available");

  tmp_solver.resize( get_nchan() );

  if (!tmp_solver[ichan])
  {
    const PolnCalibratorExtension::Transformation* xform 
      = poln_extension->get_transformation (ichan);

    tmp_solver[ichan] = new TmpSolver ( xform->get_chisq(),
					xform->get_nfree(),
					xform->get_nfit() );
  }

  return tmp_solver[ichan];
}

MEAL::LeastSquares* Pulsar::PolnCalibrator::get_solver (unsigned ichan)
{
  const PolnCalibrator* thiz = this;
  return const_cast<MEAL::LeastSquares*> (thiz->get_solver (ichan));
}

void Pulsar::PolnCalibrator::setup_transformation () const try
{
  if (receiver)
    Pauli::basis().set_basis( receiver->get_basis() );
  const_cast<PolnCalibrator*>(this)->calculate_transformation();
}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::setup_transformation";
}

//! Derived classes can create and fill the transformation array
void Pulsar::PolnCalibrator::calculate_transformation () try
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::calculate_transformation" << endl;

  if (!poln_extension)
  {
    cerr << "Pulsar::PolnCalibrator::calculate_transformation no poln_extension" << endl;

    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::calculate_transformation",
		 "no PolnCalibratorExtension available");
  }

  unsigned nchan = poln_extension->get_nchan();

  DEBUG("PolnCalibrator::calculate_transformation this=" << (void*)this << " nchan=" << nchan);

  transformation_resize (nchan);

  if (poln_extension->get_has_covariance())
    covariance.resize(nchan);

  for (unsigned i=0; i < nchan; i++)
  {
    transformation[i] = new_transformation (poln_extension, i);
    if (poln_extension->get_has_covariance())
      poln_extension->get_transformation(i)->get_covariance(covariance[i]);
  }
}
catch (Error& error) {
  throw error += "Pulsar::PolnCalibrator::calculate_transformation";
}

void Pulsar::PolnCalibrator::transformation_resize (unsigned nchan)
{
  transformation.resize (nchan);
  invalid_reason.resize (nchan);
}

/*!  
  The unitary component of each Jones matrix is parameterized by a
  half angle, i.e. -pi/2 < angle < pi/2, and any wrap between +/- pi/2
  will appear to the Fourier transform as a discontinuity in phase.

  To compensate for this, the unitary component of each Jones matrix
  is first squared, so that the angle spans 2pi smoothly.  The result
  is then decomposed and the square root of each unitary component is
  taken before recomposing the final result.
*/
template<typename T>
void interpolate( std::vector< Jones<T> >& to,
		  const std::vector< Jones<T> >& from,
		  const vector<bool>* bad )
{
  const complex<T> zero (0.0);

  complex<T> determinant;
  Quaternion<T, Hermitian> hermitian;
  Quaternion<T, Unitary> unitary;

  unsigned ichan = 0;
  const unsigned nchan_from = from.size();
  const unsigned nchan_to = to.size();

  vector< Jones<T> > copy (nchan_from);

  for (ichan=0; ichan<nchan_from; ichan++)
  {
    if (det(from[ichan]) == zero)
    {
      copy[ichan] = from[ichan];
    }
    else
    {
      polar (determinant, hermitian, unitary, from[ichan]);
      unitary *= unitary;
      copy[ichan] = determinant * (hermitian * unitary);
    }
  }

  fft::interpolate (to, copy);
  
  double factor = nchan_from / double(nchan_to);
  
  for (ichan=0; ichan<nchan_to; ichan++)
  {
    unsigned orig_ichan = ichan * factor;

    if (orig_ichan >= nchan_from || (bad && (*bad)[orig_ichan]))
    {
      to[ichan] = 0;
      continue;
    }
    
    polar (determinant, hermitian, unitary, to[ichan]);
    unitary = sqrt(unitary);

    // I don't remember why I did this, sorry ... WvS 16 Oct 2008
    if (unitary[0] < 0.05)
      polar (determinant, hermitian, unitary, from[orig_ichan]);

    to[ichan] = determinant * (hermitian * unitary);
  }
}

void Pulsar::PolnCalibrator::build_response ()
{
  response.resize( transformation.size() );
  bad.resize ( transformation.size() );
  bad_count = 0;

  for (unsigned ichan=0; ichan < response.size(); ichan++)
  {
    bad[ichan] = false;

    if (transformation[ichan])
    {
      // sanity check of model parameters
      unsigned nparam = transformation[ichan]->get_nparam();
      for (unsigned iparam=0; iparam < nparam; iparam++)
        if ( !myfinite(transformation[ichan]->get_param(iparam)) )
	{
	  if (verbose > 2)
	    cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan
		 << " " << transformation[ichan]->get_param_name(iparam)
		 << " not finite" << endl;

	  bad[ichan] = true;
        }

      double normdet = norm(det( transformation[ichan]->evaluate() ));

      if ( !myfinite(normdet) )
      {
	if (verbose > 2)
	  cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
	    " non-finite determinant" << endl;

	bad[ichan] = true;
      }
      else if ( normdet < minimum_determinant )
      {
	if (verbose > 2)
	  cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
	    " determinant=" << normdet <<
	    " less than threshold=" << minimum_determinant << endl;

	bad[ichan] = true;
      }
      else
      {
        response[ichan] = transformation[ichan]->evaluate();

        if (verbose > 2)
          cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan <<
            " response=\n" << response[ichan] << endl;
      }
    }
    else
    {
      if (verbose > 2) cerr << "Pulsar::PolnCalibrator::build ichan=" << ichan 
                        << " no transformation" << endl;

      bad[ichan] = true;
    }

    if (bad[ichan])
    {
      response[ichan] = 0.0;
      bad_count ++;
    }
  }
}

void Pulsar::PolnCalibrator::patch_response ()
{
  unsigned nchan = response.size();

  for (unsigned ichan = 0; ichan < nchan; ichan++)
  {
    if (!bad[ichan])
      continue;

    unsigned ifind;
	
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::patch_response ichan="
	   << ichan << endl;
      
    // find preceding good
    for (ifind=nchan+ichan-1; ifind > ichan; ifind--)
      if (!bad[ifind%nchan])
	break;
    
    if (ifind == ichan)
      throw Error (InvalidState, "Pulsar::PolnCalibrator::patch_response",
		   "no good data preceding ichan=%u", ichan);
      
    Jones<float> left = response[ifind%nchan];
      
    // find next good
    for (ifind=ichan+1; ifind < nchan+ichan; ifind++)
      if (!bad[ifind%nchan])
	break;
      
    if (ifind == nchan+ichan)
      throw Error (InvalidState, "Pulsar::PolnCalibrator::patch_response",
		   "no good data following ichan=%u", ichan);
    
    Jones<float> right = response[ifind%nchan];
      
    // TO-DO: It is probably more accurate to first form Mueller
    // matrices and average these as a function of frequency, in order
    // to describe any instrumental bandwidth depolarization.
    
    response[ichan] = float(0.5) * (left + right);
  }
}


void Pulsar::PolnCalibrator::build (unsigned nchan) try
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build transformation size="
	 << transformation.size() << " nchan=" << nchan << endl;

  bool transformation_built = transformation.size() > 0;

  unsigned maximum_nchan = get_maximum_nchan ();

  /*
    If the derived class can shrink the transformation array, then
    it may be necessary to rebuild the transformation array to match
    the number of frequency channels in the observation
  */

  if (maximum_nchan)
  {
    /*
      target: the number of channels in the observation, up to the 
      maximum number of channels supported by the derived class.
    */
    unsigned target_nchan = std::min (nchan, maximum_nchan);

    if (transformation.size() != target_nchan)
      transformation_built = false;
  }

  if (!transformation_built)
  {
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build setup transformation nchan=" << nchan << endl;

    observation_nchan = nchan;
    setup_transformation();
  }

  if (!nchan)
    nchan = transformation.size();

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build nchan=" << nchan 
	 << " transformation.size=" << transformation.size() << endl;

  build_response ();

  /* If there are bad channels and the solution must be interpolated to
     a higher number of frequency channels, then the solution must be 
     patched up */

  bool need_patching = bad_count && nchan > response.size();

  if (median_smoothing)
  {
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
  else if (interpolating || need_patching)
  {
    if (!interpolating)
      cerr << "Pulsar::PolnCalibrator::build patch up"
	" before interpolation in Fourier domain" << endl;

    patch_response ();
  }

  if (nchan < response.size())
  {
    // TO-DO: It is probably more accurate to first form Mueller
    // matrices and average these as a function of frequency, in order
    // to describe any instrumental bandwidth depolarization.

    unsigned factor = response.size() / nchan;
    if (nchan * factor != response.size())
      cerr << "Pulsar::PolnCalibrator::build WARNING calibrator nchan="
	   << response.size() << " mod requested nchan=" << nchan << " != 0" 
	   << endl;

    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build integrate by " << factor << endl;

    // integrate the Jones matrices
    scrunch (response, factor, true);
  }

  if (nchan > response.size())
  {
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build interpolating from nchan="
	   << response.size() << " to " << nchan << endl;

    vector< Jones<float> > out_response (nchan);
    interpolate (out_response, response, &bad);
    response = out_response;
  }

  Jones<double> feed_xform = 1.0;

  // if known, add the feed transformation
  if (feed)
  {
    feed_xform = feed->get_transformation();
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build known feed:\n"
           << feed_xform << endl;
  }

#define CORRECT_BASIS 0
#if CORRECT_BASIS

  /*
    WvS - 14 May 2012

    It is confusing for the PolnCalibrator class to correct only the
    basis component of the frontend transformation based on the
    receiver extension obtained from the calibrator archive while the
    projection component of the frontend is computed based on the
    receiver extension obtained from the pulsar archive.

    In fact, before today, if the basis_projection_corrected flag was
    set to true in the calibrator archive, the PolnCalibrator class
    would add an incorrectly computed basis transformation, leading to
    bug #3526460.
  */

  Jones<double> rcvr_xform = 1.0;

  // if known, add the receiver basis transformation
  if (receiver)
  {
    BasisCorrection basis_correction;
    rcvr_xform = basis_correction (receiver);
    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::build known receiver basis:\n"
           << rcvr_xform << endl;
  }

#endif

  for (unsigned ichan=0; ichan < nchan; ichan++)
  {
    if (!get_valid(ichan))
    {
      response[ichan] = 0.0;
      continue;
    }

    // add the known feed transformation
    response[ichan] *= feed_xform;

#if CORRECT_BASIS
    // add the known receiver transformation
    response[ichan] *= rcvr_xform;
#endif

    // invert: the response must undo the effect of the instrument
    response[ichan] = inv (response[ichan]);
  }

  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::build built" << endl;

  built = true;
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::build";
}

float Pulsar::PolnCalibrator::get_weight (unsigned ichan) const
{
  return (get_valid(ichan)) ? 1.0 : 0.0;
}

//! Return true if the response for the specified channel is valid
bool Pulsar::PolnCalibrator::get_valid (unsigned ichan) const
{
  if (ichan >= response.size())
    return false;

  static const complex<float> zero (0.0);
  return det(response[ichan]) != zero;
}

void Pulsar::PolnCalibrator::calibration_setup (const Archive* arch)
{
  string reason;
  if (!get_calibrator()->calibrator_match (arch, reason))
    throw Error (InvalidParam, "Pulsar::PolnCalibrator::calibration_setup",
		 "mismatch between calibrator\n\t" 
		 + get_calibrator()->get_filename() +
                 " and\n\t" + arch->get_filename() + reason);

  if (response.size() != arch->get_nchan()) try
  {
    if (variation)
    {
      DEBUG("PolnCalibrator::calibration_setup calling Variation::update");
      variation->update (arch->get_Integration(0));
    }
    
    build( arch->get_nchan() );
  }
  catch (Error& error)
  {
    throw error += "Pulsar::PolnCalibrator::calibration_setup";
  }
}

/*! Upon completion, the flux of the archive will be normalized with
  respect to the flux of the calibrator, such that a FluxCalibrator
  simply scales the archive by the calibrator flux. */
void Pulsar::PolnCalibrator::calibrate (Archive* arch) try
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::calibrate" << endl;

  calibration_setup (arch);

  if (arch->get_npol() == 4 || arch->get_npol() == 2)
  {
    if (do_backend_correction)
    {
      BackendCorrection correct_backend;
      correct_backend (arch);
    }

    if (verbose > 2)
      cerr << "Pulsar::PolnCalibrator::calibrate Archive::transform" <<endl;

    unsigned nsubint = arch->get_nsubint();
    
    for (unsigned isub=0; isub < nsubint; isub++)
    {
      Integration* subint = arch->get_Integration (isub);

      if (variation)
      {
	bool rebuild_needed = variation->update (subint);
	if (rebuild_needed)
	  build (subint->get_nchan());
      }
      
      subint->expert()->transform (response);
    }
    
    arch->set_poln_calibrated (true);

#if CORRECT_BASIS
    if (receiver)
    {
      Receiver* rcvr = arch->get<Receiver>();
      if (!rcvr)
	throw Error (InvalidState, "Pulsar::PolnCalibrator::calibrate",
		     "Archive has no Receiver Extension");
      
      rcvr->set_basis_corrected (true);
    }
#endif

  }
  else if (arch->get_npol() == 1)
  {
    if (Archive::verbose)
      cerr << "Pulsar::PolnCalibrator::calibrate WARNING"
	" calibrating only absolute gain" << endl;

    unsigned nsub = arch->get_nsubint ();
    unsigned nchan = arch->get_nchan ();

    for (unsigned isub=0; isub < nsub; isub++)
    {
      Integration* subint = arch->get_Integration (isub);
      for (unsigned ichan=0; ichan < nchan; ichan++)
      {
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

  ProcHistory* history = arch->getadd<ProcHistory>();
  if (history)
  {
    string fnames;
    for (unsigned i=0; i<filenames.size(); i++)
      fnames += " " + basename( filenames[i] );

    // erase the first space
    fnames.erase (0, 1);

    history->set_cal_file( fnames );
    history->set_cal_mthd( get_type()->get_name() );
  }

  arch->set_scale (Signal::ReferenceFluxDensity);
}
catch (Error& error)
{
  throw error += "Pulsar::PolnCalibrator::calibrate";
}


const Pulsar::Calibrator::Type* Pulsar::PolnCalibrator::get_type () const
{
  if (type)
    return type;

  if (!poln_extension)
    throw Error (InvalidState,
		 "Pulsar::PolnCalibrator::get_type",
		 "type not set and no extension available");

  return poln_extension->get_type();
}


Pulsar::CalibratorExtension*
Pulsar::PolnCalibrator::new_Extension () const
{
  if (verbose > 2)
    cerr << "Pulsar::PolnCalibrator::new_Extension" << endl;

  return new PolnCalibratorExtension (this);
}

Pulsar::Calibrator::Info* Pulsar::PolnCalibrator::get_Info () const
{
  return PolnCalibrator::Info::create (this);
}

