//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CalibratorExtension.h,v $
   $Revision: 1.4 $
   $Date: 2004/10/08 10:04:52 $
   $Author: straten $ */

#ifndef __CalibratorExtension_h
#define __CalibratorExtension_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  class Calibrator;

  //! Calibrator Extension
  /*! This base class Extension implements the storage of Calibrator data. */
  class CalibratorExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    CalibratorExtension (const char* name = "CalibratorExtension");

    //! Copy constructor
    CalibratorExtension (const CalibratorExtension&);

    //! Operator =
    const CalibratorExtension& operator= (const CalibratorExtension&);

    //! Destructor
    ~CalibratorExtension ();

    //! Clone method
    CalibratorExtension* clone () const
    { return new CalibratorExtension( *this ); }

    //! Construct from a Calibrator instance
    void build (const Calibrator*);

    //! Set the reference epoch of the calibration experiment
    void set_epoch (const MJD& epoch);
    //! Get the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Set the weight of the specified channel
    void set_weight (unsigned ichan, float weight);
    //! Get the weight of the specified channel
    float get_weight (unsigned ichan) const;

    //! Set the centre frequency of the specified channel
    void set_centre_frequency (unsigned ichan, double centre_frequency);
    //! Get the centre frequency of the specified channel
    double get_centre_frequency (unsigned ichan) const;

    protected:

    //! The reference epoch of the calibration experiment
    MJD epoch;

    //! The weight of each channel
    vector<float> weight;

    //! The centre frequency of each channel
    vector<double> centre_frequency;

    //! Ensure that ichan < get_nchan
    void range_check (unsigned ichan, const char* method) const;

  };
 

}

#endif
