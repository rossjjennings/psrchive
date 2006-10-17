//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Checks/Pulsar/CalSource.h,v $
   $Revision: 1.1 $
   $Date: 2006/10/17 23:06:49 $
   $Author: straten $ */

#ifndef __Pulsar_CalSource_h
#define __Pulsar_CalSource_h

#include "Pulsar/Correction.h"

namespace Pulsar {
  
  //! Corrects the Signal::Source type attribute of the Archive
  /*! If the Archive::type attribute is set to Signal::PolnCal and the
    Archive::source attribute is in any of the recognized lists, then
    the Archive::type attribute may be corrected. */
  class CalSource : public Correction {

  public:
    
    //! Default constructor
    CalSource ();
      
    // Return the name of the check
    std::string get_name () { return "CalSource"; }

    //! Correct the Signal::Source type attribute of the Archive
    void apply (Archive* archive);

  protected:

    //! Source names matching FluxCalOn
    std::vector<std::string> fluxcal_on;

    //! Source names matching FluxCalOff
    std::vector<std::string> fluxcal_off;

  };
 

}

#endif
