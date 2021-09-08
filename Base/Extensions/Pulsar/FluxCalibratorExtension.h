//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/FluxCalibratorExtension.h

#ifndef __FluxCalibratorExtension_h
#define __FluxCalibratorExtension_h

#include "Pulsar/CalibratorExtension.h"

namespace Pulsar
{

  class FluxCalibrator;
  //! Flux Calibrator Extension
  /*! This Extension implements the storage of FluxCalibrator data. */

  class FluxCalibratorExtension : public CalibratorExtension
  {

    friend class FluxCalibrator;

  public:

    //! Default constructor
    FluxCalibratorExtension ();

    //! Copy constructor
    FluxCalibratorExtension (const FluxCalibratorExtension&);

    //! Operator =
    const FluxCalibratorExtension& operator= (const FluxCalibratorExtension&);

    //! Destructor
    ~FluxCalibratorExtension ();

    //! Clone method
    FluxCalibratorExtension* clone () const
      { return new FluxCalibratorExtension( *this ); }

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    // Text interface to a FluxCalibratorExtension
    class Interface : public TextInterface::To<FluxCalibratorExtension>
    {
      public:
	Interface( FluxCalibratorExtension *s_instance = NULL );
    };

    //! Construct from a FluxCalibrator instance
    FluxCalibratorExtension (const FluxCalibrator*);

    //! Return a short name
    std::string get_short_name () const { return "fcal"; }

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);

    //! Get the number of frequency channels
    unsigned int get_nchan( void ) const;

    //! Remove the inclusive range of channels
    void remove_chan (unsigned first, unsigned last);

    //! Set the number of receptors
    void set_nreceptor (unsigned nreceptor);

    //! Get the number of receptors
    unsigned get_nreceptor () const;

    //! Set the system equivalent flux density of the specified channel
    void set_S_sys (unsigned chan, unsigned receptor, const Estimate<double>&);
    //! Get the system equivalent flux density of the specified channel
    Estimate<double> get_S_sys (unsigned chan, unsigned receptor) const;
    //! Get the system equivalent flux densities of the fluxcal 
    std::vector< std::vector< Estimate<double> > > get_S_sys () const;

    //! Set the calibrator flux density of the specified channel
    void set_S_cal (unsigned chan, unsigned receptor, const Estimate<double>&);
    //! Get the calibrator flux density of the specified channel
    Estimate<double> get_S_cal (unsigned chan, unsigned receptor) const;
    //! Get the calibrator flux densities of the fluxcal 
    std::vector< std::vector< Estimate<double> > > get_S_cal () const;

    //! Returns true if scale and gain_ratio data are available
    bool has_scale () const { return scale_available; }
    void has_scale (bool flag) { scale_available = flag; }

    //! Set the flux scale of the specified channel
    void set_scale (unsigned chan, unsigned receptor, const Estimate<double>&);
    //! Get the flux scale of the specified channel
    Estimate<double> get_scale (unsigned chan, unsigned receptor) const;

    //! Set the gain ratio of the specified channel
    void set_gain_ratio (unsigned chan, unsigned r, const Estimate<double>&);
    //! Get the gain ratio of the specified channel
    Estimate<double> get_gain_ratio (unsigned chan, unsigned receptor) const;

    //! Get the number of parameters that describe each solution
    unsigned get_nparam () const;

    //! Get the specified parameter for the specified channel
    Estimate<float> get_Estimate ( unsigned iparam, unsigned ichan ) const;
    void set_Estimate ( unsigned iparam, unsigned ichan, const Estimate<float>& );

    bool get_valid (unsigned ichan) const;
    void set_valid (unsigned ichan, bool valid);

    //! Append FluxCalibrator Extension data from another Archive
    void frequency_append (Archive* to, const Archive* from);

  protected:

    //! System equivalent flux density in each receptor and frequency channel
    /*! in mJy */
    std::vector< std::vector< Estimate<double> > > S_sys;

    //! Calibrator flux density in each receptor and frequency channel
    /*! in mJy */
    std::vector< std::vector< Estimate<double> > > S_cal;

    //! Flag set if the following data are available
    bool scale_available;

    //! Flux scale in each receptor and frequency channel
    /*! in mJy per uncalibrated flux density unit */
    std::vector< std::vector< Estimate<double> > > scale;

    //! Gain ratio in each receptor and frequency channel
    /*! unitless */
    std::vector< std::vector< Estimate<double> > > ratio;

  };


}

#endif
