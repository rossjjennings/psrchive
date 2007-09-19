//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ProfileAmpsExpert.h,v $
   $Revision: 1.1 $
   $Date: 2007/09/19 13:35:19 $
   $Author: straten $ */

#ifndef __Pulsar_ProfileAmpsExpert_h
#define __Pulsar_ProfileAmpsExpert_h

#include "Pulsar/ProfileAmps.h"

namespace Pulsar {

  //! Provides access to private and protected members of ProfileAmps
  /*! 
    This class provides the required access without the need to:
    <UL>
    <LI> unprotect methods, or
    <LI> make friends with every class or function that requires access
    </UL>
  */

  class ProfileAmps::Expert {

  public:

    Expert (ProfileAmps* inst)
    { instance = inst; }

    //! Set the amplitudes pointer
    void set_amps_ptr (float* amps) const
    { instance->amps = amps; }

  private:

    //! instance
    ProfileAmps* instance;

  };

}

#endif
