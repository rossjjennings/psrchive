//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ZapInterpreter.h,v $
   $Revision: 1.2 $
   $Date: 2007/05/24 08:42:30 $
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

    //! zap data using the specified algorithm
    std::string zap (const std::string& args);

  protected:

    //! Zap median algorithm
    Reference::To<ChannelZapMedian> zap_median;

  };

}

#endif
