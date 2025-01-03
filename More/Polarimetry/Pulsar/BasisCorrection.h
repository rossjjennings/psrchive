//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/BasisCorrection.h

#ifndef __Pulsar_BasisCorrection_H
#define __Pulsar_BasisCorrection_H

#include "Reference.h"
#include "Jones.h"

namespace Pulsar
{
  class Archive;
  class Receiver;

  //! Corrects the basis, based on FD_POLN, FD_HAND and FD_SANG
  /*! FD_POLN, FD_HAND and FD_SANG are PSRFITS parameter names described
      in Table 1 of van Straten, Manchester, Johnston & Reynolds (2010) */
  class BasisCorrection : public Reference::Able
  {
  public:

    //! Return true if basis correction is required
    bool required (const Archive*) const;

    //! Return true if basis correction is required
    bool required (const Receiver*) const;

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

    //! Return a summary of parameters that produce non-identity terms
    std::string get_summary () const;

  protected:
    mutable std::string summary;

  };
}

#endif
