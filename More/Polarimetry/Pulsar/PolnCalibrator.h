//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolnCalibrator.h,v $
   $Revision: 1.9 $
   $Date: 2003/05/05 10:48:38 $
   $Author: straten $ */

#ifndef __PolnCalibrator_H
#define __PolnCalibrator_H

#include <string>

#include "Calibrator.h"
#include "Estimate.h"
#include "Jones.h"
#include "Types.h"

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
    //! If true, then the human-readable parameters are kept
    static bool store_parameters;

    //! Construct from an vector of PolnCal Pulsar::Archives
    PolnCalibrator (const vector<Archive*>& archives);

    //! Construct from an single PolnCal Pulsar::Archive
    PolnCalibrator (const Archive* archive);

    //! Destructor
    virtual ~PolnCalibrator ();

    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);

    //! Build the model at the native frequency resolution
    virtual void build ();

    //! Return the mean levels of the calibrator hi and lo states
    void get_levels (unsigned isubint, unsigned nchan, 
		     vector<vector<Estimate<double> > >& cal_hi,
		     vector<vector<Estimate<double> > >& cal_lo) const;

    //! Return a const reference to the calibrator archive
    const Archive* get_Archive () const { return calibrator; }

  protected:

    //! Return the system response as determined by the output CAL states
    /*! Given the coherency products (and cross-products) of the
      calibrator hi and lo states, derived classes must return the
      Jones matrix that represents the system response.  If the derived
      class can store additional parameters, the ichan parameter may
      be used. */
    virtual Jones<double> solve (const vector<Estimate<double> >& hi,
				 const vector<Estimate<double> >& lo,
				 unsigned ichan) = 0;

    //! Resize the space used to store additional parameters
    /*! If the derived class can store additional parameters, the space
      should be resized here */
    virtual void resize_parameters (unsigned nchan) {}

    //! Filenames of Pulsar::Archives from which instance was created
    vector<string> filenames;

    //! Intensity of off-pulse (system + sky), in CAL flux units
    vector< Estimate<double> > baseline;

    //! Jones matrix frequency response of the instrument
    vector< Jones<float> > jones;

    //! Reference to the Pulsar::Archive from which this instance was created
    Reference::To<const Archive> calibrator;

    //! Create the Jones matrix frequency response at the requested resolution
    void create (unsigned nchan);

    //! Calculate the Jones matrix frequency response from the CAL hi and lo
    void calculate (vector<vector<Estimate<double> > >& cal_hi,
		    vector<vector<Estimate<double> > >& cal_lo);

  };

}

#endif
