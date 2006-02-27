//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/ReferenceCalibrator.h,v $
   $Revision: 1.6 $
   $Date: 2006/02/27 19:33:25 $
   $Author: straten $ */

#ifndef __Pulsar_ReferenceCalibrator_H
#define __Pulsar_ReferenceCalibrator_H

#include "Pulsar/PolnCalibrator.h"
#include "Estimate.h"
#include "Stokes.h"
#include "Types.h"

namespace Pulsar {

  class Integration;

  //! Calibrators derived from reference source observations
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
                     std::vector<std::vector<Estimate<double> > >& hi,
                     std::vector<std::vector<Estimate<double> > >& lo) const;

    //! Return the mean levels of the calibrator hi and lo states
    static void get_levels (const Archive* archive, unsigned nchan,
		            std::vector<std::vector<Estimate<double> > >& hi,
		            std::vector<std::vector<Estimate<double> > >& lo);

    //! Return the mean levels of the calibrator hi and lo states
    static void get_levels (const Integration* integration, unsigned nchan,
		            std::vector<std::vector<Estimate<double> > >& hi,
		            std::vector<std::vector<Estimate<double> > >& lo);

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::PolnCalibrator over-ride
    //
    // ///////////////////////////////////////////////////////////////////

    //! Set the number of frequency channels in the response array
    virtual void set_nchan (unsigned nchan);

  protected:

    //! Intensity of off-pulse (system + sky), in CAL flux units
    std::vector< Estimate<double> > baseline;

    //! The Stokes parameters of the reference source
    Stokes< Estimate<double> > reference_source;

    //! Filter access to the calibrator
    void set_calibrator (const Archive* archive);

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
    void calculate (std::vector<std::vector<Estimate<double> > >& hi,
		    std::vector<std::vector<Estimate<double> > >& lo);

    //! Derived classes must perform the actual solution
    virtual MEAL::Complex2* 
    solve (const std::vector<Estimate<double> >& hi,
	   const std::vector<Estimate<double> >& lo) = 0;

    //! Derived classes may also compute other things
    virtual void extra (unsigned ichan,
			const std::vector< Estimate<double> >& source,
			const std::vector< Estimate<double> >& sky) { }

  };

}

#endif
