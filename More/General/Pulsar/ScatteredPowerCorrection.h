//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ScatteredPowerCorrection.h,v $
   $Revision: 1.1 $
   $Date: 2006/02/20 18:40:05 $
   $Author: straten $ */

#ifndef __Pulsar_ScatteredPowerCorrection_h
#define __Pulsar_ScatteredPowerCorrection_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Integration;

  //! Corrects the power scattered due to 2-bit quantization
  /*! Computes the average S/N based on profile baseline mean and
    integration length */
  class ScatteredPowerCorrection : public Transformation<Integration> {

  public:

    //! Default constructor
    ScatteredPowerCorrection ();

    //! The scattered power correction operation
    void transform (Integration*);

  protected:

  };

}

#endif
