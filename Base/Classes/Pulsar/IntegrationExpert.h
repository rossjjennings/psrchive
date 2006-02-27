//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/IntegrationExpert.h,v $
   $Revision: 1.1 $
   $Date: 2006/02/27 19:44:38 $
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

    //! Swap the two specified Profiles
    void swap_profiles (unsigned ipol, unsigned ichan,
			unsigned jpol, unsigned jchan)
    { instance->swap_profiles (ipol, ichan, jpol, jchan); }

  private:

    //! instance
    Reference::To<Integration, false> instance;

  };

}

#endif
