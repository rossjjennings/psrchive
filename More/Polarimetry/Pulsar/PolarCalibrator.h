//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/PolarCalibrator.h,v $
   $Revision: 1.13 $
   $Date: 2004/11/22 21:32:30 $
   $Author: straten $ */

#ifndef __PolarCalibrator_H
#define __PolarCalibrator_H

#include "Pulsar/ReferenceCalibrator.h"
#include "MEAL/Polar.h"

namespace Pulsar {

  //! Uses PolarModel to represent the system response
  class PolarCalibrator : public ReferenceCalibrator {
    
  public:

    //! Construct from an single PolnCal Pulsar::Archive
    PolarCalibrator (const Archive* archive);

    //! Destructor
    ~PolarCalibrator ();

    //! Return Calibrator::Polar
    Type get_type () const;

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

      //! Return the scale of parameters in the specified class
      float get_scale (unsigned iclass) const;

    };

    //! Return the PolarAxisCalibrator information
    Info* get_Info () const;

  protected:

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::ReferenceCalibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Return a new MEAL::Polar instance
    ::MEAL::Complex2* solve (const vector<Estimate<double> >& hi,
				    const vector<Estimate<double> >& lo);


  };

}

#endif
