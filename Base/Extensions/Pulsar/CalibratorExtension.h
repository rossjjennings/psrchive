//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CalibratorExtension.h,v $
   $Revision: 1.6 $
   $Date: 2004/10/11 14:26:44 $
   $Author: straten $ */

#ifndef __CalibratorExtension_h
#define __CalibratorExtension_h

#include "Pulsar/Archive.h"
#include "Pulsar/Calibrator.h"

namespace Pulsar {

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

    //! Set the type of the calibrator
    virtual void set_type (Calibrator::Type type);
    //! Get the type of the calibrator
    Calibrator::Type get_type () const;

    //! Set the reference epoch of the calibration experiment
    virtual void set_epoch (const MJD& epoch);
    //! Get the reference epoch of the calibration experiment
    MJD get_epoch () const;

    //! Set the number of frequency channels
    virtual void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Set the weight of the specified channel
    virtual void set_weight (unsigned ichan, float weight);
    //! Get the weight of the specified channel
    float get_weight (unsigned ichan) const;

    //! Set the centre frequency of the specified channel
    void set_centre_frequency (unsigned ichan, double centre_frequency);
    //! Get the centre frequency of the specified channel
    double get_centre_frequency (unsigned ichan) const;

    protected:

    //! Type of the calibrator
    Calibrator::Type type;
    
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
