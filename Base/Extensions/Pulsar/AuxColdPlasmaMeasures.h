//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010-2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Extensions/Pulsar/AuxColdPlasmaMeasures.h

#ifndef __Pulsar_AuxColdPlasmaMeasures_h
#define __Pulsar_AuxColdPlasmaMeasures_h

#include "Pulsar/IntegrationExtension.h"

namespace Pulsar {
  
  //! Stores parameters used to correct variable dispersion and birefringence
  class AuxColdPlasmaMeasures : public Pulsar::Integration::Extension {
    
  public:
    
    //! Default constructor
    AuxColdPlasmaMeasures ();

    //! Return a newly constructed copy of self
    AuxColdPlasmaMeasures* clone () const;

    //! Return a text interfaces that can be used to access this instance
    TextInterface::Parser* get_interface();

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "aux"; }

    //! Set the difference in dispersion measure
    /*! The difference is with respect to Archive::get_dispersion_measure */
    void set_dispersion_measure (double measure);
    //! Get the difference dispersion measure
    /*! The difference is with respect to Archive::get_dispersion_measure */
    double get_dispersion_measure () const;

    //! Set the difference in rotation measure
    /*! The difference is with respect to Archive::get_rotation_measure */
    void set_rotation_measure (double measure);
    //! Get the difference in rotation measure
    /*! The difference is with respect to Archive::get_rotation_measure */
    double get_rotation_measure () const;

    //! Text interface to a AuxColdPlasmaMeasures instance
    class Interface : public Integration::Extension::Interface<AuxColdPlasmaMeasures>
    {
    public:
      Interface (AuxColdPlasmaMeasures* = NULL);
    };

  protected:

    //! The difference in dispersion measure
    double dispersion_measure = 0.0;
    
    //! The difference in rotation measure
    double rotation_measure = 0.0;
  };
  

}

#endif

