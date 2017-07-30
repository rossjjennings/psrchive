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

#include "Pulsar/Index.h"
#include "TextInterface.h"
#include "PhaseRange.h"

namespace Pulsar {

  class Archive;
  class Integration;
  class Profile;
  class ProfileStats;
  class PhaseWeight;
  class SNRatioEstimator;
  class WidthEstimator;
  
  //! Interface to a variety of useful statistics
  class Statistics : public Reference::Able
  {

  public:

    //! Default constructor
    Statistics (const Archive* = 0);

    //! Set the instance from which statistics will be drawn
    void set_Archive (const Archive*);
    //! Get the instance from which statistics will be drawn
    const Archive* get_Archive () const;

    //! Set the sub-integration from which statistics will be drawn
    void set_subint (Index _isubint);
    Index get_subint () const;
    
    //! Set the frequency channel from which statistics will be drawn
    void set_chan (Index _ichan);
    Index get_chan () const;

    //! Set the polarization to plot
    void set_pol (Index _ipol);
    Index get_pol () const;

    //! Set the signal-to-noise ratio estimator
    void set_snr_estimator (const std::string& name);

    //! Get the signal-to-noise ratio
    double get_snr () const;

    //! Get the text interface of the signal-to-noise ratio estimator
    TextInterface::Parser* get_snr_interface ();

    //! Set the pulse width estimator
    void set_pulse_width_estimator (const std::string& name);

    //! Get the pulse width
    Phase::Value get_pulse_width () const;

    //! Get the text interface of the pulse width estimator   
    TextInterface::Parser* get_pulse_width_interface ();
    
    //! Get the effective duty cycle
    double get_effective_duty_cycle () const;

    //! Get the Fourier-noise-to-noise ratio
    double get_nfnr () const;

    //! Get the number of cal transitions
    unsigned get_cal_ntrans () const;

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

    Reference::To<const Archive, false> archive;
    Index isubint;
    Index ichan;
    Index ipol;

    bool stats_setup;
    void setup_stats ();
    mutable Reference::To<ProfileStats> stats;

    const Profile* get_Profile () const;
    mutable Reference::To<const Profile, false> profile;

    const Integration* get_Integration () const;
    mutable Reference::To<const Integration, false> integration;

    mutable std::vector< Reference::To<Plugin> > plugins;

    Reference::To<SNRatioEstimator> snr_estimator;
    Reference::To<WidthEstimator> pulse_width_estimator;

    //! Set the period attribute of the Phase::Value/Range/Ranges argument
    void set_period (Phase::HasUnit&) const;

  };

  class Statistics::Plugin : public Reference::Able
  {
  public:
    virtual void setup () = 0;
    virtual TextInterface::Parser* get_interface () = 0;

    const Integration* get_Integration ()
    { return parent->get_Integration(); }

  protected:
    friend class Statistics;
    Reference::To<Statistics, false> parent;
  };

}

// standard interface constructor defined in More/General/standard_interface.C
TextInterface::Parser* standard_interface (Pulsar::Archive*);

// also defined in More/General/standard_interface.C
std::string process (TextInterface::Parser* interface, const std::string& txt);

#endif
