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

class RobustEstimateZapper;

namespace Pulsar {

  class ChannelZapMedian;
  class Mower;
  class TimeFrequencyZap;
  class ZapExtend;

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

    //! zap outliers using time/freq plane methods
    std::string tfzap (const std::string& args);

    //! zap the specified channels
    std::string chan (const std::string& args);

    //! zap the specified integrations
    std::string subint (const std::string& args);

    //! zap the specified integration,channel
    std::string such (const std::string& args);

    //! zap the specified fraction of the band at the edges
    std::string edge (const std::string& args);

    //! zap the band edges up to the specified fraction of the cumulative power
    std::string cumulative (const std::string& args);

    //! zap the specified ranges of frequencies
    std::string freq (const std::string& args);

    //! extend zapped regions in time/freq
    std::string extend (const std::string& args);

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

    //! Time/Frequency zapping algorithm
    Reference::To<TimeFrequencyZap> tf_zapper;
    
    //! Zap extender
    Reference::To<ZapExtend> zap_extend;

    //! Robust estimate zapper
    Reference::To<RobustEstimateZapper> robust_estimate_zapper;
 
    //! Store specified set of subints to apply channel/freq zapping
    std::vector<unsigned> set_subints;

    //! Apply channel/freq zapping to any calibrator extension
    bool zap_calibrator;
  };

}

#endif
