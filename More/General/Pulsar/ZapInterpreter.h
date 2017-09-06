//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ZapInterpreter.h

#ifndef __Pulsar_ZapInterpreter_h
#define __Pulsar_ZapInterpreter_h

#include "Pulsar/InterpreterExtension.h"

namespace Pulsar {

  class ChannelZapMedian;
  class Mower;
  class InterQuartileRange;
  
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

    //! zap outliers using the inter-quartile range
    std::string iqr (const std::string& args);

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

    //! toggle the flag to zap channels in calibrator
    std::string cal (const std::string& args);

    //! no empty commands
    std::string empty ();

  protected:

    //! Zap median algorithm
    Reference::To<ChannelZapMedian> zap_median;

    //! Mowing algorithm
    Reference::To<Mower> mower;

    //! Inter-quartile range algorithm
    Reference::To<InterQuartileRange> iq_range;
    
    //! Store specified set of subints to apply channel/freq zapping
    std::vector<unsigned> set_subints;

    //! Apply channel/freq zapping to any calibrator extension
    bool zap_calibrator;
  };

}

#endif
