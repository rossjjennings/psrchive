//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/ITRFExtension.h,v $
   $Revision: 1.1 $
   $Date: 2003/06/12 18:40:00 $
   $Author: straten $ */

#ifndef __ITRFExtension_h
#define __ITRFExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! International Terrestrial Reference Frame Extension
  /*! This ITRFExtension class provides a container for
    holding the ITRF coordinates of a telescope. */

  class ITRFExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    ITRFExtension ();
    
    //! Copy constructor
    ITRFExtension (const ITRFExtension& extension);
    
    //! Operator =
    const ITRFExtension& operator= (const ITRFExtension& extension);
    
    //! Destructor
    ~ITRFExtension ();
    
    ////////////////////////////////////////////////////////////////

    //! Antenna ITRF X-coordinate
    double ant_x;
    
    //! Antenna ITRF Y-coordinate
    double ant_y;
    
    //! Antenna ITRF Z-coordinate
    double ant_z;

  };
 

}

#endif
