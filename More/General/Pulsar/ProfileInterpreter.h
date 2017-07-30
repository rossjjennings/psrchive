//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ProfileInterpreter.h

#ifndef __Pulsar_ProfileInterpreter_h
#define __Pulsar_ProfileInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class Smooth;
  class RemoveBaseline;

  class ProfileInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    ProfileInterpreter ();

    //! Destructor
    ~ProfileInterpreter ();

    //! Remove the profile baseline
    std::string baseline (const std::string& args);

    //! Form the cummulative profile
    std::string cumulative (const std::string& args);

    //! Form the difference profile
    std::string difference (const std::string& args);

    //! Form the mean smoothed profile
    std::string mean (const std::string& args);

    //! Form the median smoothed profile
    std::string median (const std::string& args);

    //! Form the low-pass filtered profile
    std::string sinc (const std::string& args);

    //! Extract the specified phase bin range
    std::string extract (const std::string& args);

    //! Subtract the named archive from the current
    std::string subtract (const std::string& args);

    //! Convolve the named archive with the current
    std::string convolve (const std::string& args);

    //! Correlate the named archive with the current
    std::string correlate (const std::string& args);

    //! No empty arguments
    std::string empty ();

  protected:

    std::string smooth (Smooth*, const std::string& args);

    Reference::To<RemoveBaseline> remove_baseline;
  };

}

#endif
