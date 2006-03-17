//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ArchiveExpert.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/17 13:34:50 $
   $Author: straten $ */

#ifndef __Pulsar_ArchiveExpert_h
#define __Pulsar_ArchiveExpert_h

#include "Pulsar/IntegrationManagerExpert.h"
#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Provides access to private and protected member of Archive
  /*! Some methods of the Archive class are protected or private only
    so that they may not be naively called.  This class provides 
    access to developers without the need to:
    <UL>
    <LI> unprotect methods, or
    <LI> make friends with every class or function that requires access
    </UL>
  */
  class Archive::Expert : public IntegrationManager::Expert {

  public:

    Expert (Archive* inst) : IntegrationManager::Expert (inst)
    { instance = inst; }

    //! Set the number of pulsar phase bins
    void set_nbin (unsigned numbins)
    { instance->set_nbin (numbins); }

    //! Set the number of frequency channels
    void set_nchan (unsigned numchan)
    { instance->set_nchan (numchan); }

    //! Set the number of polarization measurements
    void set_npol (unsigned numpol)
    { instance->set_npol (numpol); }

    //! Load the specified Integration from filename, returning new instance
    Integration* load_Integration (const char* filename, unsigned subint)
    { return instance->load_Integration (filename, subint); }

    //! Initialize an Integration to reflect Archive attributes.
    void init_Integration (Integration* subint)
    { instance->init_Integration (subint); }

    //! Provide Integration::resize access to Archive-derived classes
    void resize_Integration (Integration* integration)
    { instance->resize_Integration (integration); }
    
    //! Apply the current model to the Integration
    void apply_model (Integration* subint, const polyco* old = 0)
    { instance->apply_model (subint, old); }

  private:

    //! instance
    Reference::To<Archive, false> instance;

  };

}

#endif
