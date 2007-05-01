//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Checks/Pulsar/CalSource.h,v $
   $Revision: 1.2 $
   $Date: 2007/05/01 05:49:50 $
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

    //! Get the number of FluxCalOn aliases
    unsigned get_on_size () const { return fluxcal_on.size(); }

    //! Get the number of FluxCalOff aliases
    unsigned get_off_size () const { return fluxcal_off.size(); }

    //! Get the filename from which FluxCalOn aliases were read
    std::string get_on_filename () const { return on_filename; }

    //! Get the filename from which FluxCalOff aliases were read
    std::string get_off_filename () const { return off_filename; }

  protected:

    //! Source names matching FluxCalOn
    std::vector<std::string> fluxcal_on;

    //! Source names matching FluxCalOff
    std::vector<std::string> fluxcal_off;

    //! Name of file from which FluxCalOn aliases were read
    std::string on_filename;

    //! Name of file from which FluxCalOff aliases were read
    std::string off_filename;

  };
 

}

#endif
