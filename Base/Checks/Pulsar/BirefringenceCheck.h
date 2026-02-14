//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Checks/Pulsar/BirefringenceCheck.h

#ifndef __Pulsar_BirefringenceCheck_h
#define __Pulsar_BirefringenceCheck_h

#include "Pulsar/Verification.h"

namespace Pulsar {
  
  //! Verifies that each Integration has a consistent BirefringenceHistory Extension
  /*! If the Archive::faraday_corrected attribute is set then, for each
    Integration, ensure that
    <OL>
    <LI> the BirefringenceHistory Extension has been added;
    <LI> BirefringenceHistory::rotation_measure == Archive::rotation_measure; and
    <LI> BirefringenceHistory::reference_frequency == Archive::centre_frequency.
    </OL>
  */
  class BirefringenceCheck : public Verification
  {
  public:
    
    //! Return the name of the check
    std::string get_name () { return "BirefringenceCheck"; }

    //! Perform the check
    void apply (const Archive* archive);

    //! Check the correction relative to the centre frequency
    void check_relative (const Archive* archive, unsigned isubint);

    //! Check the correction relative to infinite frequency
    void check_absolute (const Archive* archive, unsigned isubint);
  };

}

#endif
