//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/PolnCalibratorExtension.h,v $
   $Revision: 1.1 $
   $Date: 2003/09/12 11:50:34 $
   $Author: straten $ */

#ifndef __PolnCalibratorExtension_h
#define __PolnCalibratorExtension_h

#include "Calibration/Transformation.h"
#include "Pulsar/Calibration.h"
#include "Pulsar/Archive.h"

namespace Pulsar {
  
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

    //! Set the type of the instrumental response parameterization
    void set_type (CalibratorType type);
    //! Get the type of the instrumental response parameterization
    CalibratorType get_type () const;

    //! Set the name of the instrumental response parameterization
    void set_name (const string& name);
    //! Get the name of the instrumental response parameterization
    string get_name () const;

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);
    //! Get the number of frequency channels
    unsigned get_nchan () const;

    //! Get the transformation for the specified frequency channel
    ::Calibration::Transformation* get_Transformation (unsigned c);
    //! Get the transformation for the specified frequency channel
    const ::Calibration::Transformation* get_Transformation (unsigned c) const;

  protected:
    
    //! Type of the instrumental response parameterization
    CalibratorType type;
    
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
