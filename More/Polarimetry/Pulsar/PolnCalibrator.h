//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnCalibrator.h,v $
   $Revision: 1.25 $
   $Date: 2003/12/26 08:44:11 $
   $Author: straten $ */

#ifndef __Pulsar_PolnCalibrator_H
#define __Pulsar_PolnCalibrator_H

#include "Pulsar/Calibrator.h"
#include "Calibration/Complex2.h"
#include "Jones.h"
#include "MJD.h"

#ifdef sun
#include <ieeefp.h>
#endif

namespace Pulsar {

  class Integration;
  class PolnCalibratorExtension;

  //! Base class of polarization calibration objects.
  /*! New convention: Polarimetric calibration no longer depends on a
    flux calibrator in order to work.  The calibrated archive will
    have its flux normalized by the calibrator flux, such that the
    FluxCalibrator class need only multiply the archive by the
    calibrator flux in mJy. */
  class PolnCalibrator : public Calibrator {

  public:

    //! Construct with optional processed calibrator Archive
    PolnCalibrator (Archive* archive = 0);

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
    unsigned get_transformation_nchan () const;

    //! Return true if the transformation for the specified channel is valid
    bool get_transformation_valid (unsigned ch) const;

    //! Set the transformation invalid flag for the specified channel
    void set_transformation_invalid (unsigned ch);

    //! Return the transformation for the specified channel
    const ::Calibration::Complex2* get_transformation (unsigned ichan) const;

    //! Return the transformation for the specified channel
    ::Calibration::Complex2* get_transformation (unsigned ichan);

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);

    //! Return the Calibrator::Type of derived class
    virtual Type get_type () const;

    //! Return the reference epoch of the calibration experiment
    virtual MJD get_epoch () const;

    //! Return an Archive with a PolnCalibratorExtension
    virtual Archive* get_solution (const string& archive_class,
				   string filename_extension = ".pc") const;

    //! Base class generalizes PolnCalibrator parameter communication
    class Info : public Calibrator::Info {

    public:

      //! Factory returns a suitable instance
      static Calibrator::Info* create (const PolnCalibrator* calibrator);

      //! Constructor
      Info (const PolnCalibrator* calibrator);
      
      //! Return the number of parameter classes
      unsigned get_nclass () const;

      //! Return the name of the specified class
      const char* get_name (unsigned iclass) const;
      
      //! Return the number of parameters in the specified class
      unsigned get_nparam (unsigned iclass) const;
      
      //! Return the estimate of the specified parameter
      Estimate<float> get_param (unsigned ichan, unsigned iclass,
				 unsigned iparam) const;
      
      //! Return the colour index
      int get_colour_index (unsigned iclass, unsigned iparam) const;

      //! Return the graph marker
      int get_graph_marker (unsigned iclass, unsigned iparam) const;

    protected:

      //! The PolnCalibrator to be plotted
      Reference::To<const PolnCalibrator> calibrator;

      //! The number of parameters in the PolnCalibrator transformation
      unsigned nparam;

    };

    //! Return the Calibrator::Info information
    /*! By default, derived classes need not necessarily define Info */
    Calibrator::Info* get_Info () const;


  protected:

    //! The array of transformation Model instances
    vector< Reference::To< ::Calibration::Complex2 > > transformation;
    
    //! Derived classes can create and fill the transformation array
    virtual void calculate_transformation ();

    //! The array of Jones matrices derived from the transformation array
    vector< Jones<float> > response;

    //! The PolnCalibratorExtension of the Archive passed during construction
    Reference::To<PolnCalibratorExtension> extension;

  private:

    //! Build the response array
    void build (unsigned nchan = 0);

  };

}

#endif

