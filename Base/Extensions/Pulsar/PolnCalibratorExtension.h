//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/PolnCalibratorExtension.h,v $
   $Revision: 1.6 $
   $Date: 2003/10/08 14:17:38 $
   $Author: straten $ */

#ifndef __PolnCalibratorExtension_h
#define __PolnCalibratorExtension_h

#include "Calibration/Transformation.h"
#include "Pulsar/Calibrator.h"
#include "Pulsar/Archive.h"

namespace Pulsar {

  class PolnCalibrator;

  //! PolnCalibrator Extension
  /*! This Extension implements the storage of PolnCalibrator data. */
  
  class PolnCalibratorExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    PolnCalibratorExtension ();

    //! Copy constructor
    PolnCalibratorExtension (const PolnCalibratorExtension&);

    //! Operator =
    const PolnCalibratorExtension& operator= (const PolnCalibratorExtension&);

    //! Destructor
    ~PolnCalibratorExtension ();

    //! Clone method
    PolnCalibratorExtension* clone () const
    { return new PolnCalibratorExtension( *this ); }

    //! Construct from a PolnCalibrator instance
    PolnCalibratorExtension (const PolnCalibrator*);

    //! Set the type of the instrumental response parameterization
    void set_type (Calibrator::Type type);
    //! Get the type of the instrumental response parameterization
    Calibrator::Type get_type () const;

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Return true if the transformation for the specified channel is valid
    bool get_valid (unsigned ichan) const;

    //! Get the transformation for the specified frequency channel
    ::Calibration::Transformation* get_Transformation (unsigned c);
    //! Get the transformation for the specified frequency channel
    const ::Calibration::Transformation* get_Transformation (unsigned c) const;

  protected:
    
    //! Type of the instrumental response parameterization
    Calibrator::Type type;
    
    //! The instrumental response as a function of frequency
    vector< Reference::To< ::Calibration::Transformation > > response;

    //! Return a new Transformation instance, based on type attribute
    ::Calibration::Transformation* new_Transformation ();

    //! Construct the response array according to the current attributes
    void construct ();

    //! Ensure that ichan < get_nchan
    void range_check (unsigned ichan, const char* method) const;

  };
 

}

#endif
