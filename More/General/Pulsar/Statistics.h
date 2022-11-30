//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Statistics.h

#ifndef __Pulsar_Statistics_h
#define __Pulsar_Statistics_h

#include "Pulsar/HasArchive.h"
#include "TextInterface.h"
#include "PhaseRange.h"

namespace Pulsar {

  class ProfileStats;
  class PhaseWeight;
  class StrategySet;
  
  //! Interface to a variety of useful statistics
  class Statistics : public Reference::Able, public HasArchive
  {

  public:

    //! Default constructor
    Statistics (const Archive* = 0);

    //! Get the strategy manager owned by the Archive
    StrategySet* get_strategy () const;

    //! Get the phase of the pulse peak
    Phase::Value get_peak () const;
 
    //! Get the effective duty cycle
    double get_effective_duty_cycle () const;

    //! Get the Fourier-noise-to-noise ratio
    double get_nfnr () const;

    //! Get the number of cal transitions
    unsigned get_cal_ntrans () const;

    //! Get the number of profiles that have zero weight
    unsigned get_nzero () const;

    //! Get Kish's design effect
    double get_design_effect () const;

    //! Get the predicted level of 2-bit distortion
    double get_2bit_dist () const;
    
    //! Get the off-pulse baseline
    PhaseWeight* get_baseline ();

    //! Get the on-pulse phase bins
    PhaseWeight* get_onpulse ();

    //! Get all phase bins
    PhaseWeight* get_all ();

    //! Get the Profile statistics interface
    ProfileStats* get_stats ();

    //! Get the weighted frequency from the Pulsar::Archive
    double get_weighted_frequency () const;

    //! Get the width of a phase bin in seconds
    double get_bin_width () const;

    //! Get the dispersive smearing in the worst channel in seconds
    double get_dispersive_smearing () const;

    //! Text interface to statistics
    class Interface;

    //! Get the text interface to this
    TextInterface::Parser* get_interface ();

    //! Pure virtual base class of statistics plugins
    class Plugin;

    //! Add a plugin
    void add_plugin (Plugin*);

  protected:

    void setup_stats ();
    mutable Reference::To<ProfileStats> stats;

    mutable std::vector< Reference::To<Plugin> > plugins;

    //! Set the period attribute of the Phase::Value/Range/Ranges argument
    void set_period (Phase::HasUnit&) const;

  };

  class Statistics::Plugin : public Reference::Able
  {
  public:

    Plugin () { setup = false; }

    virtual TextInterface::Parser* get_interface () = 0;

    const Integration* get_Integration ()
    { return parent->get_Integration(); }

  protected:
    friend class Statistics;
    Reference::To<Statistics, false> parent;
    virtual void set_setup (bool flag) { setup = flag; }
    mutable bool setup;
  };

}

// standard interface constructor defined in More/General/standard_interface.C
TextInterface::Parser* standard_interface (Pulsar::Archive*);

// also defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

#endif
