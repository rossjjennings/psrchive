//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CalibratorExtension.h,v $
   $Revision: 1.1 $
   $Date: 2003/09/11 21:21:32 $
   $Author: straten $ */

#ifndef __CalibratorExtension_h
#define __CalibratorExtension_h

#include "Calibration/Transformation.h"
#include "Pulsar/Calibration.h"
#include "Pulsar/Archive.h"

// using Calibration::Transformation;

namespace Pulsar {
  
  //! Calibrator Extension
  /*! This Extension implements the storage of Calibrator data. */
  
  class CalibratorExtension : public Pulsar::Archive::Extension {
    
  public:
    
    //! Default constructor
    CalibratorExtension ();

    //! Copy constructor
    CalibratorExtension (const CalibratorExtension&);

    //! Operator =
    const CalibratorExtension& operator= (const CalibratorExtension&);

    //! Destructor
    ~CalibratorExtension ();

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
    ::Calibration::Transformation* get_Transformation (unsigned ch);
    //! Get the transformation for the specified frequency channel
    const ::Calibration::Transformation* get_Transformation (unsigned ch) const;

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
