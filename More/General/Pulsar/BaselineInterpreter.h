//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/BaselineInterpreter.h,v $
   $Revision: 1.2 $
   $Date: 2007/10/02 05:18:49 $
   $Author: straten $ */

#ifndef __Pulsar_BaselineInterpreter_h
#define __Pulsar_BaselineInterpreter_h

#include "CommandParser.h"
#include "Functor.h"

namespace Pulsar {

  class PhaseWeight;
  class Profile;

  class BaselineInterpreter : public CommandParser {

  public:

    //! Default constructor
    BaselineInterpreter ();

    //! Install the interative Gaussian baseline algorithm
    std::string normal (const std::string& args);

    //! Install the default baseline algorithm (minimum window)
    std::string minimum (const std::string& args);

    //! No empty arguments
    std::string empty ();

  protected:

    Functor< PhaseWeight* (const Profile*) > normal_functor;
    Functor< PhaseWeight* (const Profile*) > minimum_functor;

  };

}

#endif
