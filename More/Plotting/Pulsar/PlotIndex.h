//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/Attic/PlotIndex.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/15 17:59:32 $
   $Author: straten $ */

#ifndef __Pulsar_PlotIndex_h
#define __Pulsar_PlotIndex_h

#include <iostream>

namespace Pulsar {

  //!
  class PlotIndex {

  public:

    //! Default constructor
    PlotIndex (unsigned value = 0, bool integrate = false);

    //! Get the value of the index
    unsigned get_value () const { return index; }
    //! Set the value of the index
    void set_value (unsigned value) { index = value; }

    //! Get the integrate flag
    bool get_integrate () const { return integrate; }
    //! Set the integrate flag
    void set_integrate (bool flag) { integrate = flag; }

  protected:

    unsigned index;
    bool integrate;

  };

  std::ostream& operator << (std::ostream& os, const PlotIndex&);
  std::istream& operator >> (std::istream& is, PlotIndex&);

  class Profile;
  class Archive;

  //! Return the requested profile, cloning and integrating when needed
  const Profile*
  get_Profile (const Archive* data,
	       PlotIndex subint, PlotIndex pol, PlotIndex chan);

  class PolnProfile;

  //! Return a new PolnProfile with state == Signal::Stokes
  /*! This function clones, integrates, & converts only when necessary */
  const PolnProfile* 
  get_Stokes (const Archive* data, PlotIndex subint, PlotIndex chan);

}

#endif
