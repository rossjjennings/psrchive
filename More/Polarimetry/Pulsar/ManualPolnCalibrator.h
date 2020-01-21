/***************************************************************************
 *
 *   Copyright (C) 2013 by Gregory Desvignes
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __CalibrationManualPolnCalibrator_H
#define __CalibrationManualPolnCalibrator_H

#include "Pulsar/PolnCalibrator.h"
#include "Jones.h"

namespace Pulsar {

  //class ReferenceCalibrator;
  //class CalibratorStokes;

  //! Phenomenological description of the instrument
  /*! 
     */
  class ManualPolnCalibrator : public PolnCalibrator {

  public:

    //! Default Constructor
    ManualPolnCalibrator (const std::string ascii_model_filename);

    //! Copy Constructor
    ManualPolnCalibrator (const ManualPolnCalibrator& s);

    //! Assignment Operator
    //const ManualPolnCalibrator& operator = (const ManualPolnCalibrator& s);

    //! Clone operator
    //ManualPolnCalibrator* clone () const;

    //! Destructor
    ~ManualPolnCalibrator ();

    virtual void load (const std::string& ascii_model_filename);

    // ///////////////////////////////////////////////////////////////////
    //
    // useful for calibrating
    //
    // ///////////////////////////////////////////////////////////////////

    //! Set the number of frequency channels in the response array
    virtual void set_response_nsub (unsigned nsub);

    //! Get the number of frequency channels in the response array
    virtual unsigned get_response_nsub () const;

    //! Return the system response for the specified channel
    //virtual Jones<float> get_response (unsigned ichan) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Model implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return the name of the class
    //std::string get_name () const;
    
    //! Calibrate the polarization of the given archive
    void calibrate (Archive* archive);

    class Entry {

    public:

      //! Null constructor
      Entry () { init(); }

      //! Construct from an ASCII string
      explicit Entry (std::string& str) { load(str); }
    
      //! Destructor
      ~Entry();

      //! load from ascii string
      void load (const std::string& str);

      //! return true if the criteria matches
      bool matches (const std::string& name) const;

      double ref_frequency; 

      MJD ref_epoch; 

      void set_response (const Jones<float> response_h) {entry_response = response_h;}

      Jones<float> get_response () {return entry_response;}

    private:
    
    protected:
 
      //! Clean slate
      void init ();

    private:
      
      //double ref_frequency; 

      //!
      //MJD ref_epoch; 

      Jones<float> entry_response;
    };

    //void set_response (const Jones<float> response_h) {response = response_h;}

    //Jones<float> get_response () {return response;}
    
    //! Returns the best match, given the source name and centre frequency
    std::vector<Entry> match (const MJD& epoch) const;
    const Entry& match (const MJD& epoch, double MHz) const;

    //! Returns a given entry
    Entry get_entry (const unsigned idx) const { return entries[idx]; }


  protected:

    //! 
    std::vector<std::vector<Jones<float> > > response;

    //! vector of entries in the database
    std::vector<Entry> entries;

    std::vector<Entry> matches_epoch (const MJD& epoch) const;

  private:
    
    //! Name of the file from which the entries were loaded
    std::string ascii_model_filename;

    //! Initialize function used by constructors
    //void init ();



  };

}

#endif

