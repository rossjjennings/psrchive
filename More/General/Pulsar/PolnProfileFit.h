//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/PolnProfileFit.h,v $
   $Revision: 1.7 $
   $Date: 2004/04/21 01:41:03 $
   $Author: ahotan $ */

#ifndef __Pulsar_PolnProfileFit_h
#define __Pulsar_PolnProfileFit_h

#include <memory>

#include "Calibration/Axis.h"
#include "Estimate.h"
#include "Stokes.h"
#include "toa.h"

// forward declarations
namespace Calibration {
  class Complex2;
  class Polynomial;
  class ReceptionModel;
}

namespace Pulsar {

  class PolnProfile;
  class Profile;

  //! Implements polarimetric pulse profile template fitting
  /*! This class may be used to find the full polarimetric best-fit
    between observed and standard pulse profiles.  The fit yields both
    the polarimetric transformation as well as the phase shift between
    the two profiles, and may be used both to calculate arrival time
    estimates and to calibrate the instrumental response. */

  class PolnProfileFit : public Reference::Able {
    
  public:

    //! Default constructor
    PolnProfileFit ();

    //! Copy constructor
    PolnProfileFit (const PolnProfileFit& fit);

    //! Assignment operator
    PolnProfileFit& operator = (const PolnProfileFit& fit);

    //! Destructor
    ~PolnProfileFit ();

    //! Set the maximum number of harmonics to include in fit
    void set_maximum_harmonic (unsigned max);

    //! Set the standard to which observations will be fit
    void set_standard (const PolnProfile* standard);

    //! Set the transformation between the standard and observation
    void set_transformation (Calibration::Complex2* xform);

    //! Fit the specified observation to the standard
    void fit (const PolnProfile* observation);

    //! Get the phase offset between the observation and the standard
    Estimate<double> get_phase () const;

    //! Set the phase offset between the observation and the standard
    void set_phase (const Estimate<double>& phase);

    //! Get the arrival time estimate
    Tempo::toa get_toa (const PolnProfile* observation,
			const MJD& mjd, double period, char nsite);

    //! Return the Fourier Transform of the PolnProfile
    PolnProfile* fourier_transform (const PolnProfile* input) const;
    
    //! Return the variance in each of the four Stokes parameters
    Stokes<float> get_variance (const PolnProfile* input) const;

    //! Return the phase shift based on the cross correlation function
    float ccf_max_phase (const Profile* std, const Profile* obs) const;

  protected:

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! The number of harmonics in the fit
    unsigned n_harmonic;

    //! The standard to which observations will be fit
    Reference::To<const PolnProfile> standard;

    //! The fourier transform of the standard
    Reference::To<const PolnProfile> standard_fourier;

    //! The transformation between the standard and observation
    Reference::To<Calibration::Complex2> transformation;

    //! The measurement equation used to model the fit
    Reference::To<Calibration::ReceptionModel> model;

    //! The polynomial that describes linear phase in the Fourier domain
    Reference::To<Calibration::Polynomial> phase;

    //! The phase transformation in the Fourier domain
    Reference::To<Calibration::Complex2> phase_xform;

    //! The phase axis
    Calibration::Axis<double> phase_axis;

    //! The variance of the standard
    Stokes<float> standard_variance;

    //! The power of the standard
    double standard_power;

    //! Construtor helper
    void init ();

  };

}

#endif

