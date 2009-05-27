//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/FourthMoments.h,v $
   $Revision: 1.1 $
   $Date: 2009/05/27 21:23:53 $
   $Author: straten $ */

#ifndef __Pulsar_FourthMoments_h
#define __Pulsar_FourthMoments_h

#include "Pulsar/MoreProfiles.h"

namespace Pulsar
{
  /*! Phase-resolved fourth-order moments of the electric field */
  class FourthMoments : public MoreProfiles
  {
  public:

    //! Construct with a name
    FourthMoments (const char* name = "FourthMoments");
    
    //! multiplies each bin of the profile by scale
    void scale (double scale);

    //! offsets each bin of the profile by offset
    void offset (double offset);

    //! integrate neighbouring phase bins in profile
    void bscrunch (unsigned nscrunch);

    //! integrate neighbouring phase bins in profile
    void bscrunch_to_nbin (unsigned nbin);

    //! integrate neighbouring sections of the profile
    void fold (unsigned nfold);

  };

}

#endif
