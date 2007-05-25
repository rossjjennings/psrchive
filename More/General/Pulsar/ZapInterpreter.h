//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ZapInterpreter.h,v $
   $Revision: 1.3 $
   $Date: 2007/05/25 10:59:55 $
   $Author: straten $ */

#ifndef __Pulsar_ZapInterpreter_h
#define __Pulsar_ZapInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class ChannelZapMedian;
  
  class ZapInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    ZapInterpreter ();

    //! Destructor
    ~ZapInterpreter ();

    //! zap data using the median smoothed filter
    std::string median (const std::string& args);

    //! zap the specified channels
    std::string chan (const std::string& args);

    //! zap the specified integrations
    std::string subint (const std::string& args);

    //! zap the specified integration,channel
    std::string such (const std::string& args);

    //! no empty commands
    std::string empty ();

  protected:

    //! Zap median algorithm
    Reference::To<ChannelZapMedian> zap_median;

  };

}

#endif
