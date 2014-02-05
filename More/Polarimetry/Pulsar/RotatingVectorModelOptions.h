//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2013 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/RotatingVectorModelOptions.h

#ifndef __Pulsar_RotatingVectorModelOptions_h
#define __Pulsar_RotatingVectorModelOptions_h

#include "CommandLine.h"
#include "MEAL/RotatingVectorModel.h"

namespace Pulsar {

  //! RotatingVectorModel command line options
  class RotatingVectorModelOptions 
  {
    bool _set_fit;
    MEAL::RotatingVectorModel* RVM;

  public:

    //! Default constructor
    RotatingVectorModelOptions ();

    //! Set the model to be interfaced
    void set_model (MEAL::RotatingVectorModel* model) { RVM = model; }

    //! Set when model parameters will be varied to search for optimal values
    void set_fit (bool fit) { _set_fit = fit; }

    //! Add options to the menu
    void add_options (CommandLine::Menu&);

  };

}

#endif
