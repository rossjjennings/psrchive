//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/DataSetManager.h

#ifndef __Pulsar_DataSetManager_H
#define __Pulsar_DataSetManager_H

#include "Pulsar/Database.h"

namespace Pulsar
{
  //! List of input data from a single pulsar and its calibrator observations
  class DataSet : public Reference::Able
  {
  public:

    DataSet ();

    //! Return true if data can be added to this data set
    bool matches (const Archive* data);

    //! Append data to list
    void add (const Archive* data);

    //! Integrate data into total
    void integrate (const Archive* data);
    
    //! Get the name of the pulsar in this data set
    const std::string& get_name () { return name; }

    //! Get the integrated total
    Archive* get_total () { return total; }
    
    //! Get the epoch of the first observation
    MJD get_start_epoch () const;

    //! Get the epoch of the last observation
    MJD get_end_epoch () const;

    double get_integration_length () const { return total_integration_length; }
    
    const std::vector< std::string >& get_calibrator_filenames () const
    { return calibrator_filenames; }

  protected:

    friend class DataSetManager;

    void update (const Archive* archive, const char* method);

    std::string name;
    double total_integration_length;
    
    std::vector< Reference::To<const Archive> > data;
    Reference::To<Archive> total;
    
    MJD start_time;
    MJD end_time;

    std::vector< std::string > calibrator_filenames;
  };
  
  //! Manages sets of data from multiple sources and a calibrator database
  /*! Each DataSet contains a list of input data from a single pulsar.
    This class manages input data from multiple pulsars using multiple
    DataSet instances that share a common calibrator database. */

  class DataSetManager : public Reference::Able
  {

  public:

    //! Default constructor
    DataSetManager ();

    //! Add to the array of system calibrators
    void manage (DataSet*);

    //! Get the number of data sets
    unsigned get_nset () const;
    
    //! Get the data set that matches the archive
    DataSet* get (const Archive* data);

    //! Get the ith data set
    DataSet* get_set (unsigned i);
    
    //! Set the calibrator database
    void set_database (Pulsar::Database* db) { database = db; }
    
    //! Return the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Append archive to the appropriate data set
    /*! Add a new dataset if needed */
    void add (const Archive* data);

    //! Integra archive into the appropriate data set
    /*! Add a new dataset if needed */
    void integrate (const Archive* data);

    //! Load empty archives from filenames
    void load ( std::vector<std::string>& filenames );
      
    //! Get the epoch of the first observation
    MJD get_start_epoch () const;

    //! Get the epoch of the last observation
    MJD get_end_epoch () const;

    double get_integration_length () const { return total_integration_length; }

    //! Add filenames of polarization reference source observations
    void add_polncals (DataSet*);

    //! hours from mid-time within which PolnCal observations will be selected
    void set_polncal_hours (float hours) { polncal_hours = hours;}

    // days from mid-time within which FluxCalOn observations will be selected
    void set_fluxcal_days (float days) { fluxcal_days = days;}

    // look for PolnCal observations with nearby sky coordinates
    void set_check_coordinates (bool f) { check_coordinates = f; }

    void find_poln_calibrators ();
    void find_on_flux_calibrators ();
    void find_off_flux_calibrators ();

    unsigned get_polncal_count () const { return npolncal; }
    
  protected:

    //! The data sets
    std::vector< Reference::To<DataSet> > datasets;

    //! The database from which PolnCal and FluxCal
    Reference::To<Pulsar::Database> database;

    // hours from mid-time within which PolnCal observations will be selected
    float polncal_hours;

    // days from mid-time within which FluxCalOn observations will be selected
    float fluxcal_days;

    // look for PolnCal observations with nearby sky coordinates
    bool check_coordinates;

    MJD start_time;
    MJD end_time;

    double total_integration_length;

    void find_flux_calibrators (Signal::Source, const char* short_names);

    template <class Method>
    void incorporate (const Archive* data, Method method);

    // total number of PolnCal observations
    unsigned npolncal;
  };

}

#endif

