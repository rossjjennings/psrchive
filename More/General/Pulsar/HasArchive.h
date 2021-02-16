//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/HasArchive.h

#ifndef __Pulsar_HasArchive_h
#define __Pulsar_HasArchive_h

#include "Pulsar/Index.h"
#include "Reference.h"

namespace Pulsar {

  class Archive;
  class Integration;
  class Profile;
  
  //! Manages an archive and its index interface
  class HasArchive 
  {

  public:

    //! Default constructor
    HasArchive ();

    //! Desctructor
    ~HasArchive ();

    //! Set the instance 
    virtual void set_Archive (const Archive*);

    //! Get the instance 
    const Archive* get_Archive () const;

    //! Set the sub-integration 
    void set_subint (Index _isubint);
    Index get_subint () const;
    
    //! Set the frequency channel 
    void set_chan (Index _ichan);
    Index get_chan () const;

    //! Set the polarization
    void set_pol (Index _ipol);
    Index get_pol () const;

    //! Text interface to indeces
    class Interface;

    //! Get the profile defined by the current indeces
    const Profile* get_Profile () const;

    //! Get the sub-integration defined by the current indeces
    const Integration* get_Integration () const;

  protected:

    Reference::To<const Archive, false> archive;
    Index isubint;
    Index ichan;
    Index ipol;

    mutable Reference::To<const Profile, false> profile;

    mutable Reference::To<const Integration, false> integration;

    //! Returns true when mutable profile and integration attributes are current
    bool is_current () const;

  };

}

#endif

