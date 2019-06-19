//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2005 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ProfileWeightFunction.h

#ifndef __Pulsar_ProfileWeightFunction_h
#define __Pulsar_ProfileWeightFunction_h

#include "Pulsar/PhaseWeightFunction.h"

namespace Pulsar {

  class Profile;

  //! PhaseWeight algorithms that receive an input Profile 
  class ProfileWeightFunction : public PhaseWeightFunction 
  {

  public:

    //! Default constructor
    ProfileWeightFunction ();

    //! Return a copy constructed instance of self
    virtual ProfileWeightFunction* clone () const = 0;

    //! Set the Profile from which the PhaseWeight will be derived
    virtual void set_Profile (const Profile*);

    //! Include only the specified phase bins for consideration
    void set_include (PhaseWeight* include);

    //! Exclude the specified phase bins from consideration
    void set_exclude (PhaseWeight* include);

    //! Return true if the specified phase bin should be considered
    bool consider (unsigned ibin) const;
    
    //! Returns a PhaseWeight with the Profile attribute set
    virtual void get_weight (PhaseWeight* weight);

    //! Convenience interface
    virtual PhaseWeight* operate (const Profile*);

    //! Construct a new ProfileWeightFunction from a string
    static ProfileWeightFunction* factory (const std::string& name_and_parse);

  protected:

    //! The Profile from which the PhaseWeight will be derived
    Reference::To<const Profile> profile;

    //! Derived classes implement the PhaseWeight calculation
    virtual void calculate (PhaseWeight* weight) = 0;

    //! Included phase bins
    Reference::To<PhaseWeight> include;

    //! Excluded phase bins
    Reference::To<PhaseWeight> exclude;

  };

  std::ostream& operator<< (std::ostream&, ProfileWeightFunction*);

  std::istream& operator>> (std::istream&, ProfileWeightFunction*&);

}


#endif // !defined __Pulsar_BaselineWeightFunction_h
