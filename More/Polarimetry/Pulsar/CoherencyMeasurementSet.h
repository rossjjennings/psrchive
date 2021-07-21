//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/CoherencyMeasurementSet.h

#ifndef __Calibration_CoherencyMeasurementSet_H
#define __Calibration_CoherencyMeasurementSet_H

#include "Pulsar/CoherencyMeasurement.h"
#include "MJD.h"
#include "Types.h"

namespace Calibration {

  //! A CoherencyMeasurement set and their coordinates
  class CoherencyMeasurementSet : public std::vector<CoherencyMeasurement>
  {

  public:

    //! Default constructor
    CoherencyMeasurementSet (unsigned transformation_index = 0);

    //! Destructor
    ~CoherencyMeasurementSet ();

    //! Set the transformation through which the measurements are made
    void set_transformation_index (unsigned index);

    //! Get the transformation through which the measurements are made
    unsigned get_transformation_index () const;

    //! Add an independent variable
    void add_coordinate (MEAL::Argument::Value* abscissa);

    //! Apply the independent variables
    void set_coordinates () const;

    //! Set the identifier
    void set_identifier (const std::string& val) { identifier = val; }

    //! Get the identifier
    const std::string& get_identifier () const { return identifier; }

    //! Set the name
    void set_name (const std::string& val) { name = val; }

    //! Get the name
    const std::string& get_name () const { return name; }

    //! Set the source
    void set_source (Signal::Source val) { source = val; }

    //! Get the source
    Signal::Source get_source () const { return source; }

    //! Set the epoch
    void set_epoch (const MJD& val) { epoch = val; }

    //! Get the epoch
    const MJD& get_epoch () const { return epoch; }

    //! Set the channel index
    void set_ichan (unsigned val) { ichan = val; }

    //! Get the channel index
    unsigned get_ichan () const { return ichan; }

  protected:

    //! Index of the transformation through which the measurements are made
    unsigned transformation_index;

    //! The coordinates shared by all measurements in the set
    std::vector< Reference::To<MEAL::Argument::Value> > coordinates;

    //! The identifier
    std::string identifier;

    //! Source name
    std::string name;
    
    //! Source code
    Signal::Source source; 

    //! Epoch of the observation
    MJD epoch;

    //! Frequency channel
    unsigned ichan;

  };

}

#endif
