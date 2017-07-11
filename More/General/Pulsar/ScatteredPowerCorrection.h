//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ScatteredPowerCorrection.h

#ifndef __Pulsar_ScatteredPowerCorrection_h
#define __Pulsar_ScatteredPowerCorrection_h

#include "Pulsar/Transformation.h"
#include "Pulsar/TwoBitStats.h"

#include "JenetAnderson98A5.h"
#include "TextInterface.h"

namespace Pulsar {

  class Archive;
  class Integration;

  //! Corrects the power scattered due to 2-bit quantization
  /*! Computes the average S/N based on profile baseline mean and
    integration length */
  class ScatteredPowerCorrection : public Transformation<Integration> {

  public:

    //! Default constructor
    ScatteredPowerCorrection ();

    //! The scattered power correction operation
    void correct (Archive*);

    //! The scattered power correction operation
    void transform (Integration*);

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface ();

    //! Text-based interface to Smooth class properties
    class Interface;

    //! Fail if dedispersion has overly smeared the scattered power
    void set_check_smearing (bool flag) { check_smearing = flag; }

    //! Fail if dedispersion has overly smeared the scattered power
    bool get_check_smearing () const { return check_smearing; }

  protected:

    //! Theoretical treatment of quantization effects
    JenetAnderson98 ja98;

    //! Inverts equation A5 of Jenet & Anderson 98
    JenetAnderson98::EquationA5 ja98_a5;

    //! The estimated thresholds at record time
    std::vector<float> thresholds;

    //! Two-bit statistics
    Reference::To<TwoBitStats> twobit_stats;

    //! Fail if dedispersion has overly smeared the scattered power
    bool check_smearing;
  };

}

#endif
