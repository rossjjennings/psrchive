//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/PolnCalibratorExtension.h,v $
   $Revision: 1.17 $
   $Date: 2006/01/20 20:50:09 $
   $Author: straten $ */

#ifndef __PolnCalibratorExtension_h
#define __PolnCalibratorExtension_h

#include "MEAL/Complex2.h"
#include "Pulsar/CalibratorExtension.h"

namespace Pulsar {

  class PolnCalibrator;

  //! Stores PolnCalibrator parameters in an Archive instance
  /*! This Archive::Extension implements the storage of PolnCalibrator
    data. */  
  class PolnCalibratorExtension : public CalibratorExtension {
    
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

    //! Set the number of frequency channels
    void set_nchan (unsigned nchan);

    //! Set the weight of the specified channel
    void set_weight (unsigned ichan, float weight);

    //! Get the number of parameters describing each transformation
    unsigned get_nparam () const;

    //! Return true if the transformation for the specified channel is valid
    bool get_valid (unsigned ichan) const;
    void set_valid (unsigned ichan, bool valid);

    //! Get the transformation for the specified frequency channel
    MEAL::Complex2* get_transformation (unsigned c);
    //! Get the transformation for the specified frequency channel
    const MEAL::Complex2* get_transformation (unsigned c) const;

  protected:

    //! The instrumental response as a function of frequency
    vector< Reference::To<MEAL::Complex2> > response;

    //! Return a new MEAL::Complex2 instance, based on type attribute
    MEAL::Complex2* new_transformation ();

    //! The number of parameters that describe the transformation
    unsigned nparam;

    //! Construct the response array according to the current attributes
    void construct ();

  };
 

}

#endif
