//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Pointing.h,v $
   $Revision: 1.2 $
   $Date: 2004/12/14 12:23:43 $
   $Author: straten $ */

#ifndef __Pulsar_Pointing_h
#define __Pulsar_Pointing_h

#include "Pulsar/Integration.h"
#include "Angle.h"

namespace Pulsar {
  
  //! Telescope pointing parameters recorded during the observation
  class Pointing : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    Pointing ();

    //! Copy constructor
    Pointing (const Pointing& extension);

    //! Assignment operator
    const Pointing& operator= (const Pointing& extension);
    
    //! Destructor
    ~Pointing ();

    //! Clone method
    Pointing* clone () const { return new Pointing( *this ); }

    //! Integrate information from another Integration
    void integrate (const Integration* subint);

    //! Update information based on the provided Integration
    void update (const Integration* subint);

    //! LST (in seconds) at subint centre
    double lst_sub;
    
    //! RA (J2000, in turns) at subint centre
    Angle ra_sub;

    //! DEC (J2000, in turns) at subint centre
    Angle dec_sub;

    //! Gal longitude (in degrees) at subint centre
    Angle glon_sub;

    //! Gal latitude (in degrees) at subint centre
    Angle glat_sub;

    //! Feed angle (in degrees) at subint centre
    Angle fd_ang;

    //! Position angle (in degrees) of feed at subint centre
    Angle pos_ang;

    //! Parallactic angle (in degrees) at subint centre
    Angle par_ang;

    //! Telescope azimuth (in degrees) at subint centre
    Angle tel_az;

    //! Telescope zenith angle (in degrees) at subint centre
    Angle tel_zen;

  };
  
}

#endif

