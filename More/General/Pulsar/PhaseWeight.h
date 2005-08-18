//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeight.h,v $
   $Revision: 1.3 $
   $Date: 2005/08/18 12:09:38 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseWeight_h
#define __Pulsar_PhaseWeight_h

#include "Reference.h"
#include "Estimate.h"

namespace Pulsar {

  class Profile;

  //! A simple tool for independently weighting pulse phase bins
  /*! The Pulsar::PhaseWeight class implements a useful set of methods
    required to create, store, and manipulate a weighted set of
    Pulsar::Profile phase bins. */
  class PhaseWeight : public Reference::Able {

  public:

    //! Default constructor
    PhaseWeight ();

    //! Copy constructor
    PhaseWeight (const PhaseWeight& weight);

    //! Copy vector of float constructor
    PhaseWeight (const std::vector<float>& weight);

    //! Destructor
    ~PhaseWeight ();
    
    //! Assignment operator
    const PhaseWeight& operator = (const PhaseWeight& weight);

    //! Forms the sum of this and weight
    const PhaseWeight& operator += (const PhaseWeight& weight);

    //! Forms the product of this and weight
    const PhaseWeight& operator *= (const PhaseWeight& weight);

    //! Array operator
    float& operator [] (unsigned i) { return weight[i]; }

    //! Resize the weights array
    void resize (unsigned nbin) { weight.resize(nbin); }

    //! Set all weights to the specified value
    void set_all (float weight);
  
    //! Retrieve the sum of all weights
    double get_weight_sum () const;

    //! Retrieve the max of all weights
    double get_weight_max () const;

    //! Retrieve the weights
    void get_weights (std::vector<float>& weights) const;

    //! Set the Profile to which the weights apply
    void set_Profile (const Profile* profile);

    //! Get the weighted mean of the Profile
    Estimate<double> get_mean () const;

    //! Get the weighted variance of the Profile
    Estimate<double> get_variance () const;

    //! Get the statistics of the weighted phase bins
    void stats (const Profile* profile,
		double* mean, double* variance=0, 
		double* varmean=0, double* varvar=0) const;

  protected:

    //! The weights
    std::vector<float> weight;

    //! The Profile to which the weights apply
    Reference::To<const Profile> profile;

    //! Flag set when the statistics have been calculated
    bool built;

    //! The weighted mean of the Profile
    Estimate<double> mean;

    //! The weighted variance of the Profile
    Estimate<double> variance;

    //! Compute the mean and variance attributes
    void build ();

  };

}


#endif // !defined __Pulsar_PhaseWeight_h
