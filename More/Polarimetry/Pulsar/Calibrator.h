//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/Calibrator.h,v $
   $Revision: 1.22 $
   $Date: 2005/09/23 18:59:58 $
   $Author: straten $ */

#ifndef __Calibrator_H
#define __Calibrator_H

#include <vector>

#include "Reference.h"
#include "Estimate.h"
#include "Jones.h"
#include "MJD.h"

template<typename T> class Jones;

namespace Pulsar {

  //! Forward declarations
  class Archive;
  class Integration;
  class CalibratorExtension;

  //! Pure virtual base class of Pulsar::Archive calibrators
  class Calibrator : public Reference::Able {
    
  public:

    //! Types of Models supported
    enum Type {
      //! Flux calibrator
      Flux,
      //! Instrumental corrections
      Corrections,
      //! Gain, differential gain and differential phase
      SingleAxis,
      //! Gain, 3-D boost, and two rotations (van Straten 2002)
      Polar,
      //! Polar decomposition (Hamaker 2000)
      Hamaker,
      //! Phenomenological decomposition, (Britton 2000)
      Britton,
      //! Hybrid composed of SingleAxis and any other Calibrator
      Hybrid
    };

    
    /** @name global linkage 
     *  In order that these static methods may be linked into plugin
     *  libraries, they are defined in Extension/PolnCalibratorExtension.C */
    //@{

    //! Convert Type to string
    static const char* Type2str (Type type);

    //! Convert string to Type
    static Type str2Type (const char* s);

    //@}

    //! Verbosity flag
    static bool verbose;

    //! Fractional bandwidth of the window used in median filter
    static float median_smoothing;

    //! Fractional bandwidth over which interpolation will be performed
    static float interpolating;

    //! Default constructor
    Calibrator ();
    
    //! Destructor
    virtual ~Calibrator ();

    //! Calibrate the Pulsar::Archive
    virtual void calibrate (Archive* archive) = 0;

    //! Return the Calibrator::Type of derived class
    virtual Type get_type () const = 0;

    //! Get the number of frequency channels in the calibrator
    virtual unsigned get_nchan () const = 0;

    //! Return a new Calibrator Extension
    virtual CalibratorExtension* new_Extension () const = 0;

    //! Return a new processed calibrator Archive with a CalibratorExtension
    Archive* new_solution (const std::string& archive_class) const;

    //! Return the reference epoch of the calibration experiment
    virtual MJD get_epoch () const;

    //! Return a string containing the file information
    virtual std::string get_filenames () const;

    //! Return a const reference to the calibrator archive
    const Archive* get_Archive () const;

    //! Pure virtual base class generalizes Calibrator parameter communication
    class Info : public Reference::Able {

    public:

      //! Destructor
      virtual ~Info () {}

      //! Return the number of parameter classes
      virtual unsigned get_nclass () const = 0;

      //! Return the name of the specified class
      virtual const char* get_name (unsigned iclass) const = 0;

      //! Return the number of parameters in the specified class
      virtual unsigned get_nparam (unsigned iclass) const = 0;

      //! Return the estimate of the specified parameter
      virtual Estimate<float> get_param (unsigned ichan, unsigned iclass,
					 unsigned iparam) const = 0;

      /** @name Optional Graphing Features
       *  These optional attributes make the graph look nice
       */
      //@{

      //! Return the colour index
      virtual int get_colour_index (unsigned iclass, unsigned iparam) const
      { return 1; }

      //! Return the graph marker
      virtual int get_graph_marker (unsigned iclass, unsigned iparam) const
      { return -1; }

      //@}

    };

    //! Return the Calibrator::Info information
    /*! By default, derived classes need not necessarily define Info */
    virtual Info* get_Info () const { return 0; }

  protected:

    //! Provide access to Integration::transform
    virtual void calibrate (Integration* integration,
			    const std::vector< Jones<float> >& response);

    //! Provide access to Integration::transform
    virtual void calibrate (Integration* integration,
			    const Jones<float>& response);

    //! Reference to the Pulsar::Archive from which this instance was created
    Reference::To<const Archive> calibrator;

    //! The CalibratorExtension of the Archive passed during construction
    Reference::To<const CalibratorExtension> extension;

    //! Filenames of Pulsar::Archives from which instance was created
    std::vector<std::string> filenames;

  };

}

#endif

