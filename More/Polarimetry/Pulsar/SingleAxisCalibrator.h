//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SingleAxisCalibrator.h,v $
   $Revision: 1.15 $
   $Date: 2004/11/22 21:32:31 $
   $Author: straten $ */

#ifndef __Pulsar_SingleAxisCalibrator_H
#define __Pulsar_SingleAxisCalibrator_H

#include "Pulsar/ReferenceCalibrator.h"
#include "Calibration/SingleAxis.h"
#include "Calibration/SingleAxisSolver.h"

namespace Pulsar {

  //! Uses SingleAxis to represent the system response
  class SingleAxisCalibrator : public ReferenceCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    SingleAxisCalibrator (const Archive* archive);

    //! Destructor
    ~SingleAxisCalibrator ();

    //! Return Calibrator::SingleAxis
    Type get_type () const;

    //! Communicates SingleAxis Model parameters
    class Info : public PolnCalibrator::Info {

    public:

      //! Constructor
      Info (const PolnCalibrator* calibrator);
      
      //! Return the number of parameter classes
      unsigned get_nclass () const;

      //! Return the name of the specified class
      const char* get_name (unsigned iclass) const;
      
      //! Return the number of parameters in the specified class
      unsigned get_nparam (unsigned iclass) const;

      //! Return the scale of parameters in the specified class
      float get_scale (unsigned iclass) const;

    };

    //! Return the SingleAxisCalibrator information
    Info* get_Info () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ReferenceCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new Calibration::SingleAxis instance
    ::MEAL::Complex2* solve (const vector<Estimate<double> >& hi,
				    const vector<Estimate<double> >& lo);

    //! Optimization for SingleAxis solve
    Reference::To< ::Calibration::SingleAxisSolver > solver;

  };

}

#endif
