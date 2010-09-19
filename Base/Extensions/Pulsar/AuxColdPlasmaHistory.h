//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Attic/AuxColdPlasmaHistory.h,v $
   $Revision: 1.1 $
   $Date: 2010/09/19 05:52:34 $
   $Author: straten $ */

#ifndef __Pulsar_AuxColdPlasmaHistory_h
#define __Pulsar_AuxColdPlasmaHistory_h

#include "Pulsar/IntegrationExtension.h"

namespace Pulsar {
  
  //! Stores parameters used to correct auxiliary dispersion and birefringence
  class AuxColdPlasmaHistory : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    AuxColdPlasmaHistory (const char* name);

    //! Set the correction measure
    void set_measure (double measure);
    //! Get the correction measure
    double get_measure () const;

    //! Set the corrected flag
    void set_corrected (bool flag);
    //! Get the corrected flag
    bool get_corrected () const;

  protected:

    //! The correction measure
    double measure;

    //! The corrected flag
    bool corrected;

  };
  
}

#endif

