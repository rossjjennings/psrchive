//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CalInfoExtension.h,v $
   $Revision: 1.1 $
   $Date: 2003/06/12 18:40:00 $
   $Author: straten $ */

#ifndef __CalInfoExtension_h
#define __CalInfoExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  //! CalInfoExtension
  /*! This class provides a simple container for storing the
    information used by the ATNF Telescope Control System
    when defining the state of a CAL observation */
  
  class CalInfoExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    CalInfoExtension ();

    //! Copy constructor
    CalInfoExtension (const CalInfoExtension& extension);

    //! Operator =
    const CalInfoExtension& operator= (const CalInfoExtension& extension);

    //! Destructor
    ~CalInfoExtension ();

    //////////////////////////////////////////////////////////////////////
    
    //! Cal mode (As defined by the ATNF TCS: OFF, SYNC, EXT1, EXT2)
    string cal_mode;
    
    //! Calibrator frequency
    double cal_frequency;
    
    //! Calibrator duty-cycle
    double cal_dutycycle;
    
    //! Calibrator phase
    double cal_phase;

  };
 

}

#endif
