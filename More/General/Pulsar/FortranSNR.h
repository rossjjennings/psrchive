//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/FortranSNR.h,v $
   $Revision: 1.1 $
   $Date: 2007/10/09 04:48:03 $
   $Author: jonathan_khoo $ */

#ifndef __Pulsar_FortranSNR_h
#define __Pulsar_FortranSNR_h

#include "ReferenceAble.h"
#include <Pulsar/Profile.h>

namespace Pulsar {


  //! Calculates the signal-to-noise ratio by fitting against a standard
  class FortranSNR : public Reference::Able {

  public:

	FortranSNR(); 

    //! Return the signal to noise ratio based on the shift
    float get_snr (const Profile* profile);

	void set_rms( float new_rms ) { rms = new_rms; rms_set = true; }

	private:
	float rms;
	bool rms_set;
  };

}

#endif

