/***************************************************************************
 *
 *   Copyright (C) 2011 by Stefan Oslowski
 *   Licensed under the Academic Free License version 2.1
 *
 ****************************************************************************/

#ifndef __Pulsar_ProfileCovariance_h
#define __Pulsar_ProfileCovariance_h

#include "Pulsar/Algorithm.h"

namespace Pulsar
{
  class Profile;
  
  /*! This pure virtual base class of ProfileCovariance defines the
   * interface by which various routines for calculating the covariance
   * matrix of the given profile can be accessed */
  class ProfileCovariance : public Algorithm
  {
  public:

    //! Return a copy constructed from instance of self
    // virtual ProfileCovariance* clone () const = 0;

    //! Get the name of class used
    virtual std::string get_name () const = 0;

    //! Adds the given Profile to the covariance matrix estimate
    virtual void add_Profile (const Profile* ) = 0;

    //! Compute the covariance matrix after all the profiles were added
    virtual void finalize () = 0;

    //! Perform eigen decomposition of covariance matrix
    virtual void eigen () = 0;

    //! Get the count of profiles used to estimate the covariance matrix
    virtual unsigned get_count () = 0;

    //! Set the count of profiles used to estimate the covariance matrix
    virtual void set_count ( unsigned ) = 0;

    //! Get the rank of covariance matrix
    virtual unsigned get_rank () = 0;

    //! Set the rank of covariance matrix
    virtual void set_rank ( unsigned ) = 0;

    //! Reset the covariance matrix calculations
    virtual void reset () = 0;

  protected:

    //! Default constructor initializes the following attributes to nil
    ProfileCovariance ();

    //! Has the covariance matrix
    bool finalized;

    //! Rank of the covariance matrix
    unsigned rank;

    //! The count of profiles used to estimate the covariance matrix
    unsigned count;

    //! Sum of weights
    double wt_sum;

    //! Sum of squard weights
    double wt_sum2;
  };
}

#endif
