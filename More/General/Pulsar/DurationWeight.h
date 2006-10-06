//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/DurationWeight.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:53 $
   $Author: straten $ */

#ifndef _Pulsar_DurationWeight_H
#define _Pulsar_DurationWeight_H

#include "Pulsar/IntegrationWeight.h"

namespace Pulsar {
  
  //! Sets the weight of each Integration according to its duration
  class DurationWeight : public IntegrationWeight {

  public:

    //! Default constructor
    DurationWeight ();

    //! Set weight equal to duration or scale weight by duration
    /*! 
      When true: the weight is set equal to the duration (weight = duration)
      When false: the weight is scaled by the duration (weight *= duration)
    */
    void set_weight_absolute (bool f) { weight_absolute = f; }
    bool get_weight_absolute () const { return weight_absolute; }

  protected:

    //! Set integration weights
    void weight (Integration* integration);

    bool weight_absolute;

  };
  
}

#endif

