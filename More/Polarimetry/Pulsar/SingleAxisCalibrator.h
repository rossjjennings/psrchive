//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/SingleAxisCalibrator.h,v $
   $Revision: 1.11 $
   $Date: 2003/10/08 14:28:56 $
   $Author: straten $ */

#ifndef __Pulsar_SingleAxisCalibrator_H
#define __Pulsar_SingleAxisCalibrator_H

#include "Pulsar/ArtificialCalibrator.h"
#include "Calibration/SingleAxis.h"

namespace Pulsar {

  //! Uses SingleAxis to represent the system response
  class SingleAxisCalibrator : public ArtificialCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    SingleAxisCalibrator (const Archive* archive);

    //! Destructor
    ~SingleAxisCalibrator ();

    //! Return Calibrator::SingleAxis
    Type get_type () const;

    //! Return the SingleAxisCalibrator information
    Calibrator::Info* get_Info () const;

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

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ArtificialCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new Calibration::SingleAxis instance
    ::Calibration::Transformation* solve (const vector<Estimate<double> >& hi,
					  const vector<Estimate<double> >& lo);

  };

}

#endif
