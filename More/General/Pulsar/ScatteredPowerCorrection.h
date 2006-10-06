//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ScatteredPowerCorrection.h,v $
   $Revision: 1.6 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef __Pulsar_ScatteredPowerCorrection_h
#define __Pulsar_ScatteredPowerCorrection_h

#include "Pulsar/Transformation.h"
#include "JenetAnderson98A5.h"

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

  protected:

    //! Theoretical treatment of quantization effects
    JenetAnderson98 ja98;

    //! Inverts equation A5 of Jenet & Anderson 98
    JenetAnderson98::EquationA5 ja98_a5;

    //! The estimated thresholds at record time
    std::vector<float> thresholds;

  };

}

#endif
