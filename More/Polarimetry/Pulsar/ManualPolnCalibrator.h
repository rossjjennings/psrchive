/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __CalibrationManualPolnCalibrator_H
#define __CalibrationManualPolnCalibrator_H

#include "Pulsar/PolnCalibrator.h"

namespace Pulsar
{
  //! Manages a table of Jones matrix elements as a function of time and frequency
  class ManualPolnCalibrator : public PolnCalibrator
  {
  public:

    //! Default Constructor loads content from specified file name
    ManualPolnCalibrator (const std::string& filename);

    //! Loads content from specified file name
    virtual void load (const std::string& filename);

    //! Calibrate the polarization of the given observation
    void calibrate (Archive* archive);

    //! Subject an observation to the transformation (inverse of calibration)
    void transform (Archive*);

    //! The response at a single radio frequency
    class Response
    {
    public:

      //! load from string and return the epoch
      /*! The elements of the Jones matrix are parsed in the following order:
          Re[j_{00}], Im[j_{00}], Re[j_{01}], Im[j_{01}], Re[j_{10}], Im[j_{10}], Re[j_{11}], Im[j_{11}]
      */
      MJD load (const std::string& str);

      //! Set the reference frequency
      void set_frequency (double f) { frequency = f; }

      //! Get the reference frequency
      double get_frequency () const { return frequency; }

      //! Set the Jones matrix
      void set_response (const Jones<float>& J) { response = J; }

      //! Get the Jones matrix
      const Jones<float>& get_response () const { return response; }

    protected:
 
      //! The reference frequency
      double frequency = 0.0; 

      //! The response funtion
      Jones<float> response;
    };

    //! The frequency response for a given epoch
    class FrequencyResponse
    {
    public:

      //! Set the reference epoch
      void set_epoch (const MJD& mjd) { epoch = mjd; }

      //! Get the reference eopch
      MJD get_epoch () const { return epoch; }

      //! Return the best-matching Response object, given the frequency in MHz
      const Response& match (double freq_MHz) const;

      //! Add a new Response object
      void add (const Response& single) { response.push_back(single); }

    protected:

      //! The reference epoch
      MJD epoch;

      //! The frequency response
      std::vector<Response> response;
    };

    //! Add a new Response at the given epoch
    void add (const MJD& epoch, const Response&);

    //! Return the best-matching frequency response, given the epoch
    const FrequencyResponse& match (const MJD& epoch) const;

    //! Return the Jones matrix for the given the epoch and frequency in MHz
    const Jones<float>& get_response (const MJD& epoch, double MHz) const;

  protected:

    //! The frequency response as a function of epoch
    std::vector<FrequencyResponse> response;

    //! Index of the current frequency response
    unsigned current_response = 0;

    //! Name of the file from which the entries were loaded
    std::string ascii_model_filename;

    //! Performs the work for calibrate and transform
    void transform_work (Archive* arch, const std::string& name, bool invert);
  };
}

#endif
