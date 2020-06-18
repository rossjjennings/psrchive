//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Simulation/Pulsar/SyntheticPolnProfile.h

#ifndef __Pulsar_SyntheticPolnProfile_h
#define __Pulsar_SyntheticPolnProfile_h

#include "MEAL/Univariate.h"
#include "MEAL/Scalar.h"

namespace Pulsar {

  class PolnProfile;
  class Profile;

  //! Simulated polarimetric pulse profile
  /*! Each function should accept the pulse phase in radians */
  class SyntheticPolnProfile : public Reference::Able {

  public:

    //! Default constructor
    SyntheticPolnProfile ();

    //! Destructor
    ~SyntheticPolnProfile ();

    //! Set the functional form of the total intensity
    void set_total_intensity (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the functional form of the degree of polarization
    void set_degree (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the constant degree of polarization
    void set_degree (float);

    //! Set the variance of the noise
    void set_noise_variance (float);

    //! Set the standard deviation of the noise
    void set_noise (float);

    //! Set the peak variance of the simulated SWIMS/jitter noise
    void set_swims_variance (float);

    //! Set the peak standard deviation of the simulated SWIMS/jitter noise
    void set_swims (float);

    //! The the off-pulse baseline mean
    void set_baseline (float);
    
    //! Set the functional form of the position angle
    void set_position_angle (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the constant position angle
    void set_position_angle (float);

    //! Set the functional form of the linear polarization
    void set_linear (MEAL::Univariate<MEAL::Scalar>*);

    //! Set the functional form of the circular polarization
    void set_circular (MEAL::Univariate<MEAL::Scalar>*);

    //! Get the polarimetric profile
    PolnProfile* get_PolnProfile (unsigned nbin) const;
    
    //! Get the polarimetric profile
    void get_PolnProfile (PolnProfile*) const;

    static void set_Profile (Profile* profile, MEAL::Univariate<MEAL::Scalar>* function);

  protected:

    //! The functional form of the total intensity
    Reference::To< MEAL::Univariate<MEAL::Scalar> > total_intensity;

    //! The variance of the noise
    float noise_variance;

    //! The peak variance of the SWIMS noise
    float swims_variance;

    //! The off-pulse baseline mean intensity
    float baseline;
    
    //! The functional form of the degree of polarization
    Reference::To< MEAL::Univariate<MEAL::Scalar> > degree;

    //! The constant degree of polarization
    float constant_degree;

    //! The functional form of the position angle
    Reference::To< MEAL::Univariate<MEAL::Scalar> > position_angle;

    //! The constant position angle
    float constant_position_angle;

    //! The functional form of the linear polarization
    Reference::To< MEAL::Univariate<MEAL::Scalar> > linear;

    //! The functional form of the circular polarization
    Reference::To< MEAL::Univariate<MEAL::Scalar> > circular;

  };

}

#endif
