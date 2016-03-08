//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/ZapInterpreter.h,v $
   $Revision: 1.6 $
   $Date: 2011/02/12 23:48:54 $
   $Author: sixbynine $ */

#ifndef __Pulsar_ZapInterpreter_h
#define __Pulsar_ZapInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class ChannelZapMedian;
  class Mower;

  class ZapInterpreter : public Interpreter::Extension {

  public:

    //! Default constructor
    ZapInterpreter ();

    //! Destructor
    ~ZapInterpreter ();

    //! zap data using the median smoothed filter
    std::string median (const std::string& args);

    //! clean data using the lawn mowing algorithm
    std::string mow (const std::string& args);

    //! zap the specified channels
    std::string chan (const std::string& args);

    //! zap the specified integrations
    std::string subint (const std::string& args);

    //! zap the specified integration,channel
    std::string such (const std::string& args);

    //! zap the specified fraction of the band at the edges
    std::string edge (const std::string& args);

    //! zap the specified ranges of frequencies
    std::string freq (const std::string& args);

    std::string zerodm (const std::string& args);

    //! no empty commands
    std::string empty ();

  protected:

    //! Zap median algorithm
    Reference::To<ChannelZapMedian> zap_median;

    //! Mowing algorithm
    Reference::To<Mower> mower;

    //! Store specified set of subints to apply channel/freq zapping
    std::vector<unsigned> set_subints;
  };

}

#endif
