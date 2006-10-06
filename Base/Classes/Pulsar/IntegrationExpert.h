//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationExpert.h,v $
   $Revision: 1.5 $
   $Date: 2006/10/06 21:05:50 $
   $Author: straten $ */

#ifndef __Pulsar_IntegrationExpert_h
#define __Pulsar_IntegrationExpert_h

#include "Pulsar/Integration.h"

namespace Pulsar {

  //! Provides access to private and protected members of Integration
  /*! Some methods of the Integration class are protected or private only
    so that they may not be naively called.  This class provides 
    access to developers without the need to:
    <UL>
    <LI> unprotect methods, or
    <LI> make friends with every class or function that requires access
    </UL>
  */
  class Integration::Expert : public Reference::Able {

  public:

    Expert (Integration* inst)
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

    //! Set the dimensions of the data container
    void resize (unsigned npol=0, unsigned nchan=0, unsigned nbin=0)
    { instance->resize (npol, nchan, nbin); }

    //! Rotate each profile by time (in seconds); updates the epoch attribute
    void rotate (double time)
    { instance->rotate (time); }

    //! Rotate each profile by phase; does not update the epoch attribute
    void rotate_phase (double phase)
    { instance->rotate_phase (phase); }

    //! Integrate profiles from neighbouring chans
    void fscrunch (unsigned nscrunch = 0)
    { instance->fscrunch (nscrunch); }

    //! Integrate profiles from single polarizations into one total intensity
    void pscrunch ()
    { instance->pscrunch (); }

    //! Swap the two specified Profiles
    void swap_profiles (unsigned ipol, unsigned ichan,
			unsigned jpol, unsigned jchan)
    { instance->swap_profiles (ipol, ichan, jpol, jchan); }

    //! Combine the data from 'from' into 'this'
    void combine (const Integration* from)
    { instance->combine (from); }

  private:

    //! instance
    Reference::To<Integration, false> instance;

  };

}

#endif
