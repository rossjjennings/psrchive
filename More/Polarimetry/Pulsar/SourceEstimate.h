//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2003 - 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SourceEstimate.h

#ifndef __Pulsar_SourceEstimate_H
#define __Pulsar_SourceEstimate_H

#include "Pulsar/MeanCoherency.h"
#include "Pulsar/ReceptionModel.h"
#include "MEAL/Coherency.h"

#include "Types.h"
#include "MJD.h"

namespace Calibration
{
  //! Manages a single source and its current best estimate (first guess)
  class SourceEstimate : public Reference::Able
  {
  public:

    //! Construct with the specified bin from Archive
    SourceEstimate (int ibin = -1);

    //! Create the source and add it to the measurement equation
    virtual void create_source (ReceptionModel* equation);

    //! Update source with the estimate
    void update ();

    //! Model of Stokes parameters 
    Reference::To<MEAL::Coherency> source;

    //! Best estimate (first guess) of Stokes parameters
    MeanCoherency estimate;

    //! Validity flag for this estimate
    bool valid;

    //! Phase bin from which pulsar polarization is derived
    int phase_bin;

    //! The index of the source in the model
    unsigned input_index;

    //! Count attempts to add data for this state
    unsigned add_data_attempts;

    //! Count failures to add data for this state
    unsigned add_data_failures;

    unsigned get_input_index() const { return input_index; }

    //! Return true if the last/top source estimate has data to constrain it
    bool is_constrained () const;

    //! Report the input data fail rate
    void report_input_failed (std::ostream&) const;

  protected:

    //! does the work for update method, which does some generic extra bits
    virtual void update_work ();
  };

  class SourceObservation
  {
  public:

    //! Source name
    std::string name;
    
    //! Source code
    Signal::Source source; 

    //! Epoch of the observation
    MJD epoch;

    void set_epoch (const MJD& mjd) { epoch = mjd; }
    const MJD& get_epoch () const { return epoch; }
    
    //! Frequency channel
    unsigned ichan;

    //! Identifier
    std::string identifier;

    void set_identifier (const std::string& mjd) { identifier = mjd; }
    const std::string& get_identifier () const { return identifier; }

    //! The observed Stokes parameters
    Stokes< Estimate<double> > observation;

    //! The baseline
    Stokes< Estimate<double> > baseline;

    //! Estimate of response at time of observation
    Jones< Estimate<double> > response;

    //! Model of response at time of observation
    Reference::To< const MEAL::Complex2 > xform;
    
  };

}

#endif
