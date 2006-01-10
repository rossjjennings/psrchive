//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnProfileFit.h,v $
   $Revision: 1.15 $
   $Date: 2006/01/10 23:35:30 $
   $Author: straten $ */

#ifndef __Pulsar_PolnProfileFit_h
#define __Pulsar_PolnProfileFit_h

#include "Pulsar/PhaseWeight.h"
#include "MEAL/Axis.h"
#include "Matrix.h"
#include "Estimate.h"
#include "Stokes.h"
#include "toa.h"

// forward declarations
namespace MEAL {
  class Complex2;
  class Polynomial;
}

namespace Calibration {
  class ReceptionModel;
  class TemplateUncertainty;
}

namespace Pulsar {

  class PolnProfile;
  class Profile;

  //! The matrix template matching algorithm
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

    //! Get the maximum number of harmonics to include in fit
    unsigned get_maximum_harmonic () const { return maximum_harmonic; }

    //! Set the standard to which observations will be fit
    void set_standard (const PolnProfile* standard);

    //! Get the standard to which observations will be fit
    const PolnProfile* get_standard () const;

    //! Set the transformation between the standard and observation
    void set_transformation (MEAL::Complex2* xform);

    //! Get the transformation between the standard and the observation
    MEAL::Complex2* get_transformation () const;

    //! Fit the specified observation to the standard
    void fit (const PolnProfile* observation);

    //! Get the phase offset between the standard and the observation
    Estimate<double> get_phase () const;

    //! Set the phase offset between the observation and the standard
    void set_phase (const Estimate<double>& phase);

    //! Get the arrival time estimate
    Tempo::toa get_toa (const PolnProfile* observation,
			const MJD& mjd, double period, char nsite);

    //! Return the Fourier Transform of the Profile
    Profile* fourier_transform (const Profile* input) const;

    //! Return the Fourier Transform of the PolnProfile
    PolnProfile* fourier_transform (const PolnProfile* input) const;
    
    //! Return the PSD of the Fourier Transform of the PolnProfile
    PolnProfile* fourier_psd (const PolnProfile* fourier) const;
    
    //! Return the variance of the fluctuation power
    double get_variance (const Profile* fourier) const;

    //! Return the variance in each of the four Stokes parameters
    Stokes<float> get_variance (const PolnProfile* fourier) const;

    //! Return the phase shift based on the cross correlation function
    float ccf_max_phase (const Profile* std, const Profile* obs) const;

    //! Set true when set_standard should choose the maximum harmonic
    bool choose_maximum_harmonic;

    //! Set true when only the total intensity should be used
    bool emulate_scalar;

  protected:

    friend class PolnProfileFitAnalysis;
    friend class ScalarProfileFitAnalysis;

    //! The maximum number of harmonics to include in the fit
    unsigned maximum_harmonic;

    //! The number of harmonics in the fit
    unsigned n_harmonic;

    //! The standard to which observations will be fit
    Reference::To<const PolnProfile> standard;

    //! The fourier transform of the standard
    Reference::To<const PolnProfile> standard_fourier;

    //! The mask used to calculate the noise power
    PhaseWeight noise_mask;

    //! The transformation between the standard and observation
    Reference::To<MEAL::Complex2> transformation;

    //! The measurement equation used to model the fit
    Reference::To<Calibration::ReceptionModel> model;

    //! Least-squares normalization includes variable template contribution
    Reference::To<Calibration::TemplateUncertainty> uncertainty;

    //! The polynomial that describes linear phase in the Fourier domain
    Reference::To<MEAL::Polynomial> phase;

    //! The phase transformation in the Fourier domain
    Reference::To<MEAL::Complex2> phase_xform;

    //! The phase axis
    MEAL::Axis<double> phase_axis;

    //! The variance of the standard
    Stokes<float> standard_variance;

    //! The total determinant of the standard
    double standard_det;

    //! The reduced chi-squared after fitting
    double reduced_chisq;

    //! Construtor helper
    void init ();

    //! Set noise mask based on power spectral density of standard
    void set_noise_mask ();

    //! Choose the maximum_harmonic for the given standard
    void choose_max_harmonic (const PolnProfile* standard_psd);

  };

}

#endif

