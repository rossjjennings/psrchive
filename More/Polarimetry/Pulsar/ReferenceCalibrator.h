//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReferenceCalibrator.h,v $
   $Revision: 1.1 $
   $Date: 2004/07/12 09:31:57 $
   $Author: straten $ */

#ifndef __Pulsar_ReferenceCalibrator_H
#define __Pulsar_ReferenceCalibrator_H

#include "Pulsar/PolnCalibrator.h"
#include "Estimate.h"
#include "Stokes.h"
#include "Types.h"

namespace Pulsar {

  class Integration;

  //! Abstract base class of Calibrators based on a single reference source
  class ReferenceCalibrator : public PolnCalibrator {
    
  public:

    //! If true, then a median filter is run on the calibrator bandpass
    static bool smooth_bandpass;

    //! Construct from an single PolnCal Pulsar::Archive
    ReferenceCalibrator (const Archive* archive);

    //! Destructor
    ~ReferenceCalibrator ();

    //! Set the Stokes parameters of the reference source
    void set_reference_source (const Stokes< Estimate<double> >& stokes);

    //! Get the Stokes parameters of the reference source
    Stokes< Estimate<double> > get_reference_source () const;

    //! Return the mean levels of the calibrator hi and lo states
    void get_levels (unsigned nchan,
                     vector<vector<Estimate<double> > >& cal_hi,
                     vector<vector<Estimate<double> > >& cal_lo) const;

    //! Return the mean levels of the calibrator hi and lo states
    static void get_levels (const Archive* archive, unsigned nchan,
		            vector<vector<Estimate<double> > >& cal_hi,
		            vector<vector<Estimate<double> > >& cal_lo);

    //! Return the mean levels of the calibrator hi and lo states
    static void get_levels (const Integration* integration, unsigned nchan,
		            vector<vector<Estimate<double> > >& cal_hi,
		            vector<vector<Estimate<double> > >& cal_lo);

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::PolnCalibrator over-ride
    //
    // ///////////////////////////////////////////////////////////////////

    //! Set the number of frequency channels in the response array
    virtual void set_nchan (unsigned nchan);

  protected:

    //! Intensity of off-pulse (system + sky), in CAL flux units
    vector< Estimate<double> > baseline;

    //! The Stokes parameters of the reference source
    Stokes< Estimate<double> > reference_source;

    //! Flag raised when the reference_source attribute is set
    bool source_set;

    //! Requested number of frequency channels
    unsigned requested_nchan;

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Solve using the observation of the reference source
    void calculate_transformation ();

    //! Does the calculation of the above
    void calculate (vector<vector<Estimate<double> > >& hi,
		    vector<vector<Estimate<double> > >& lo);

    //! Derived classes must perform the actual solution
    virtual ::Calibration::Complex2* 
    solve (const vector<Estimate<double> >& hi,
	   const vector<Estimate<double> >& lo) = 0;

  };

}

#endif
