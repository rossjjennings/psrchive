//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/MeasurementEquation.h,v $
   $Revision: 1.3 $
   $Date: 2006/10/06 21:13:54 $
   $Author: straten $ */

#ifndef __Calibration_MeasurementEquation_H
#define __Calibration_MeasurementEquation_H

#include "MEAL/CongruenceTransformation.h"

namespace Calibration {

  //! Models multiple congruence transformations of multiple inputs
  /*! This represents the propagation of multiple source states
    through multiple signal paths. */

  class MeasurementEquation : public MEAL::CongruenceTransformation
  {

  public:

    //! Default constructor
    MeasurementEquation ();

    //! Destructor
    ~MeasurementEquation ();

    //! Set the input, \f$ \rho_j \f$, where \f$ j \f$ = get_input_index
    virtual void set_input (Complex2* state);

    //! Add an input, \f$ \rho_N \f$, where \f$ N \f$ = get_num_input
    virtual void add_input (Complex2* state = 0);

    //! Get the number of input states
    unsigned get_num_input () const;

    //! Get the input index, \f$ j \f$
    unsigned get_input_index () const;

    //! Set the input index, \f$ j \f$
    void set_input_index (unsigned index);

    //! Set the transformation,\f$J_i\f$,where \f$i\f$=get_transformation_index
    virtual void set_transformation (Complex2* xform);

    //! Add a transformation, \f$J_M\f$, where \f$M\f$ = get_num_transformation
    virtual void add_transformation (Complex2* state = 0);

    //! Get the number of transformation states
    unsigned get_num_transformation () const;

    //! Get the transformation index, \f$ i \f$
    unsigned get_transformation_index () const;

    //! Set the transformation index, \f$ i \f$
    void set_transformation_index (unsigned index);

  protected:

    //! The inputs, \f$ \rho_j \f$
    std::vector< MEAL::Project<MEAL::Complex2> > inputs;

    //! The current input index, \f$ j \f$
    unsigned current_input;

    //! The transformations, \f$ J_i \f$
    std::vector< MEAL::Project<MEAL::Complex2> > xforms;

    //! The current transformation index, \f$i\f$
    unsigned current_xform;

  };

}

#endif

