//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/BasisCorrection.h,v $
   $Revision: 1.2 $
   $Date: 2008/05/28 08:32:34 $
   $Author: straten $ */

#ifndef __Pulsar_BasisCorrection_H
#define __Pulsar_BasisCorrection_H

#include "Reference.h"
#include "Jones.h"

namespace Pulsar
{
  class Archive;
  class Receiver;

  //! Correct the backend convention
  class BasisCorrection : public Reference::Able
  {
  public:

    //! Return true if the correction is required
    bool required (const Archive* archive) const;

    //! Return the basis correction transformation for the given archive
    Jones<double> operator () (const Archive*) const;

    //! Return the basis correction transformation for the given receiver
    Jones<double> operator () (const Receiver*) const;

    //! Return the transformation due to receiver hand
    Jones<double> get_hand (const Receiver*) const;

    //! Return the transformation due to receiver basis
    Jones<double> get_basis (const Receiver*) const;

    //! Return the transformation due to receiver symmetry axis
    Jones<double> get_symmetry (const Receiver*) const;
    
  };
}

#endif
