//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Attic/InterpreterVariables.h,v $
   $Revision: 1.4 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_InterpreterVariables_h
#define __Pulsar_InterpreterVariables_h

#include "Pulsar/Interpreter.h"
#include "Pulsar/ArchiveTI.h"

namespace Pulsar {

  //! The Archive Text Interface used by the Interpreter
  class Interpreter::Variables : public ArchiveTI {

  public:

    //! Default constructor
    Variables ();

    //! Get the signal-to-noise ratio
    double get_snr (const Archive*) const;

    //! Get the Fourier-noise-to-noise ratio
    double get_nfnr (const Archive*) const;

    //! Get the number of cal transitions
    unsigned get_cal_ntrans (const Archive*) const;

    //! Get the predicted level of 2-bit distortion
    double get_2bit_dist (const Archive* archive) const;

  };

}


#endif
