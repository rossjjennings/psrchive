//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/MoreProfiles.h,v $
   $Revision: 1.1 $
   $Date: 2009/05/26 07:45:34 $
   $Author: straten $ */

#ifndef __Pulsar_MoreProfiles_h
#define __Pulsar_MoreProfiles_h

#include "Pulsar/DataExtension.h"

namespace Pulsar
{
  /*! Extra pulse profiles to represent other dimensions */
  class MoreProfiles : public DataExtension
  {
  public:

    //! Construct with a name
    MoreProfiles (const char* name);
    
    //! Resize the data area
    void resize (unsigned nbin);

    //! multiplies each bin of the profile by scale
    void scale (double scale);

    //! offsets each bin of the profile by offset
    void offset (double offset);

    //! rotates the profile by phase (in turns)
    void rotate_phase (double phase);

    //! set all amplitudes to zero
    void zero ();

    //! integrate neighbouring phase bins in profile
    void bscrunch (unsigned nscrunch);

    //! integrate neighbouring phase bins in profile
    void bscrunch_to_nbin (unsigned nbin);

    //! integrate neighbouring sections of the profile
    void fold (unsigned nfold);

  protected:

    std::vector< Reference::To<Profile> > profile;

  };

}

#endif
