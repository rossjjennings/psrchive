//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/Calibrator.h,v $
   $Revision: 1.7 $
   $Date: 2003/09/11 21:15:40 $
   $Author: straten $ */

#ifndef __Calibrator_H
#define __Calibrator_H

#include <vector>

#include "Reference.h"
#include "Estimate.h"
#include "Jones.h"

template<typename T> class Jones;

namespace Pulsar {

  //! Forward declarations
  class Archive;
  class Integration;

  //! Pure virtual base class of Pulsar::Archive calibrators
  class Calibrator : public Reference::Able {
    
  public:

    //! Verbosity flag
    static bool verbose;

    //! Fractional bandwidth of the window used in median filter
    static float median_smoothing;

    //! Default constructor
    Calibrator ();
    
    //! Destructor
    virtual ~Calibrator ();

    //! Calibrate the Pulsar::Archive
    virtual void calibrate (Archive* archive) = 0;

    //! Get the number of frequency channels in the calibrator
    virtual unsigned get_nchan () const = 0;

    //! Return a const reference to the calibrator archive
    const Archive* get_Archive () const { return calibrator; }

    //! Filenames of Pulsar::Archives from which instance was created
    vector<string> filenames;

    //! Pure virtual base class generalizes Calibrator parameter communication
    class Info : public Reference::Able {

    public:

      //! Destructor
      virtual ~Info () {}

      //! Return the number of parameter classes
      virtual unsigned get_nclass () const = 0;

      //! Return the name of the specified class
      virtual const char* get_name (unsigned iclass) = 0;

      //! Return the number of parameters in the specified class
      virtual unsigned get_nparam (unsigned iclass) = 0;

      //! Return the estimate of the specified parameter
      virtual Estimate<float> get_param (unsigned ichan, unsigned iclass,
					 unsigned iparam) = 0;

    };

    //! Return the Calibrator::Info information
    /*! By default, derived classes need not necessarily define Info */
    virtual Info* get_Info () const { return 0; }

  protected:

    //! Provide access to Integration::transform
    virtual void calibrate (Integration* integration,
			    const vector< Jones<float> >& response);

    //! Reference to the Pulsar::Archive from which this instance was created
    Reference::To<Archive> calibrator;

  };

}

#endif

