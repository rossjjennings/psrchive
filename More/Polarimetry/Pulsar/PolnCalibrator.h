//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnCalibrator.h,v $
   $Revision: 1.13 $
   $Date: 2003/09/11 21:15:41 $
   $Author: straten $ */

#ifndef __Pulsar_PolnCalibrator_H
#define __Pulsar_PolnCalibrator_H

#include "Pulsar/Calibrator.h"
#include "Calibration/Transformation.h"
#include "Jones.h"

// #include "Types.h"

namespace Pulsar {

  class Integration;

  //! Abstract base class of polariation calibration objects.
  /*! New convention: Polarimetric calibration no longer depends on a
    flux calibrator in order to work.  The calibrated archive will
    have its flux normalized by the calibrator flux, such that the
    FluxCalibrator class need only multiply the archive by the
    calibrator flux in mJy. */
  class PolnCalibrator : public Calibrator {
    
  public:

    //! Constructor
    PolnCalibrator ();

    //! Copy constructor
    PolnCalibrator (const PolnCalibrator& calibrator);

    //! Destructor
    virtual ~PolnCalibrator ();

    // ///////////////////////////////////////////////////////////////////
    //
    // useful for calibrating
    //

    //! Set the number of frequency channels in the response array
    virtual void set_nchan (unsigned nchan);
    //! Get the number of frequency channels in the response array
    virtual unsigned get_nchan () const;

    //! Return the system response for the specified channel
    virtual Jones<float> get_response (unsigned ichan) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // useful for unloading
    //

    //! Get the number of frequency channels in the transformation array
    unsigned get_Transformation_nchan () const;

    //! Return the transformation for the specified channel
    virtual const ::Calibration::Transformation*
    get_Transformation (unsigned ichan) const;

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);

  protected:

    //! The array of Transformation Model instances
    vector< Reference::To< ::Calibration::Transformation > > transformation;
    
    //! Derived classes must create and fill the transformation array
    virtual void calculate_transformation () = 0;

    //! The array of Jones matrices derived from the transformation array
    vector< Jones<float> > response;

  private:

    //! Build the response array
    void build (unsigned nchan = 0);

  };

}

#endif

