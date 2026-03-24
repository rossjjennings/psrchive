//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Checks/Pulsar/DispersionCheck.h

#ifndef __Pulsar_DispersionCheck_h
#define __Pulsar_DispersionCheck_h

#include "Pulsar/Verification.h"

namespace Pulsar {
  
  //! Verifies that each Integration has a consistent DispersionHistory Extension
  /*! If the Archive::dedispersed attribute is set then, for each
    Integration, ensure that
    <OL>
    <LI> the DispersionHistory Extension has been added;
    <LI> DispersionHistory::dispersion_measure == Archive::dispersion_measure; and
    <LI> DispersionHistory::reference_frequency == Archive::centre_frequency.
    </OL>
  */
  class DispersionCheck : public Verification {

  public:
    
    //! Return the name of the check
    std::string get_name () { return "DispersionCheck"; }

    //! Perform the check
    void apply (const Archive* archive);

    //! Check the correction relative to the centre frequency
    void check_relative (const Archive* archive, unsigned isubint);

    //! Check the correction relative to infinite frequency
    void check_absolute (const Archive* archive, unsigned isubint);
  };

}

#endif
