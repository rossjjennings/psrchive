//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/ObsExtension.h,v $
   $Revision: 1.2 $
   $Date: 2003/10/08 14:17:38 $
   $Author: straten $ */

#ifndef __ObsExtension_h
#define __ObsExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! Observation Information Extension
  /*! This ObsExtension class provides a container for
    specific observation details like the name of the
    observer, the ID of the project and so on. */
  
  class ObsExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    ObsExtension ();
    
    //! Copy constructor
    ObsExtension (const ObsExtension& extension);
    
    //! Operator =
    const ObsExtension& operator= (const ObsExtension& extension);
    
    //! Destructor
    ~ObsExtension ();

    //! Clone method
    ObsExtension* clone () const { return new ObsExtension( *this ); }

    //////////////////////////////////////////////////////////////

    //! Observer name
    string observer;
    
    //! Observer's affilitation
    string affiliation;

    //! Project ID
    string project_ID;
    
    //! Name of the telescope
    string telescope;

  };
 
}

#endif
