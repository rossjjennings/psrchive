//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolarCalibrator.h,v $
   $Revision: 1.10 $
   $Date: 2003/12/26 08:44:11 $
   $Author: straten $ */

#ifndef __PolarCalibrator_H
#define __PolarCalibrator_H

#include "Pulsar/ArtificialCalibrator.h"
#include "Calibration/Polar.h"

namespace Pulsar {

  //! Uses PolarModel to represent the system response
  class PolarCalibrator : public ArtificialCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    PolarCalibrator (const Archive* archive);

    //! Destructor
    ~PolarCalibrator ();

    //! Return Calibrator::Polar
    Type get_type () const;

    //! Return the PolarAxisCalibrator information
    Calibrator::Info* get_Info () const;

    //! Communicates Polar Model parameters
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

    };

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ArtificialCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new Calibration::Polar instance
    ::Calibration::Complex2* solve (const vector<Estimate<double> >& hi,
				    const vector<Estimate<double> >& lo);


  };

}

#endif
