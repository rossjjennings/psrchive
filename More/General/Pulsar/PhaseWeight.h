//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/PhaseWeight.h,v $
   $Revision: 1.1 $
   $Date: 2004/04/27 15:28:23 $
   $Author: straten $ */

#ifndef __Pulsar_PhaseWeight_h
#define __Pulsar_PhaseWeight_h

#include "Reference.h"

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
    PhaseWeight (const vector<float>& weight);

    //! Destructor
    ~PhaseWeight ();
    
    //! Assignment operator
    const PhaseWeight& operator = (const PhaseWeight& weight);

    //! Forms the sum of this and weight
    const PhaseWeight& operator += (const PhaseWeight& weight);

    //! Forms the product of this and weight
    const PhaseWeight& operator *= (const PhaseWeight& weight);

    //! Set all weights to the specified value
    void set_all (float weight);
  
    //! Retrieve the sum of all weights
    double get_weight_sum () const;

    //! Retrieve the max of all weights
    double get_weight_max () const;

    //! Retrieve the weights
    void get_weights (vector<float>& weights) const;

    //! Get the statistics of the weighted phase bins
    void stats (const Profile* profile,
		double* mean, double* variance=0, double* varmean=0);

  protected:

    //! The weights
    vector<float> weight;

  };

}


#endif // !defined __Pulsar_PhaseWeight_h
