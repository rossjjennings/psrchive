//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Pointing.h,v $
   $Revision: 1.1 $
   $Date: 2004/07/19 12:43:20 $
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

    //! If ext is a Pointing, set attributes to mean of this and ext
    void append (Extension* ext);

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

