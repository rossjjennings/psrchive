//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Index.h

#ifndef __Pulsar_Index_h
#define __Pulsar_Index_h

#include <iostream>

namespace Pulsar {

  //! Combines an index value and integrate flag
  class Index {

  public:

    //! Default constructor
    Index (unsigned value = 0, bool integrate = false);

    //! Get the value of the index
    unsigned get_value () const { return index; }
    //! Set the value of the index
    void set_value (unsigned value) { index = value; integrate = false; }

    //! Get the integrate flag
    bool get_integrate () const { return integrate; }
    //! Set the integrate flag
    void set_integrate (bool flag = true) { integrate = flag; }

    //! Get the extension flag
    /*! Provides access to extended data sets, such as MoreProfile */
    bool get_extension () const { return extension; }
    //! Set the extension flag
    /*! Provides access to extended data sets, such as MoreProfile */
    void set_extension (bool flag = true) { extension = flag; }

  protected:

    unsigned index;
    bool integrate;
    bool extension;
  };

  std::ostream& operator << (std::ostream& os, const Index&);
  std::istream& operator >> (std::istream& is, Index&);

  class Archive;
  class Integration;

  //! Return the requested profile, cloning and integrating when needed
  const Integration*
  get_Integration (const Archive* data, Index subint);

  class Profile;

  //! Return the requested profile, cloning and integrating when needed
  const Profile*
  get_Profile (const Archive* data,
	       Index subint, Index pol, Index chan);

  const Profile*
  get_Profile (const Integration* data, Index pol, Index chan);

  class PolnProfile;

  //! Return a new PolnProfile with state == Signal::Stokes
  /*! This function clones, integrates, & converts only when necessary */
  const PolnProfile* 
  get_Stokes (const Archive* data, Index subint, Index chan);

  const PolnProfile*
  get_Stokes (const Integration* data, Index chan);
}

#endif
