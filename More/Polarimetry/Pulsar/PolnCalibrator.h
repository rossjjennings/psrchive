//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 - 2022 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/PolnCalibrator.h

#ifndef __Pulsar_PolnCalibrator_H
#define __Pulsar_PolnCalibrator_H

#include "Pulsar/Calibrator.h"
#include "Pulsar/Index.h"

#include "MEAL/LeastSquares.h"
#include "MEAL/Complex2.h"

#include "ReferenceVector.h"
#include "Jones.h"

namespace Pulsar {

  class PolnCalibratorExtension;
  class FeedExtension;
  class Receiver;
  class Integration;

  //! Polarimetric calibrators
  /*! The calibrated archive will have its flux normalized by the
    calibrator flux, such that the FluxCalibrator class need only
    multiply the archive by the absolute calibrator flux.
    Polarimetric calibration does not require a flux calibrator in
    order to work.  */
  class PolnCalibrator : public Calibrator
  {

  public:

    //! Minimum allowable determinant of Jones matrices
    /*! Set this parameter to avoid unstable inversion of matrices with
      a determinant close to zero by flagging such channels as invalid. */
    static Option<double> minimum_determinant;
    
    //! Construct with optional processed calibrator Archive
    PolnCalibrator (const Archive* archive = 0);

    //! Copy constructor
    PolnCalibrator (const PolnCalibrator& calibrator);

    //! Destructor
    virtual ~PolnCalibrator ();

    //! Set the calibrator archive used to define basic attributes
    void set_calibrator (const Archive* archive);

    //! Set the sub-integration index
    void set_subint (const Index& isub);

    // ///////////////////////////////////////////////////////////////////
    //
    // useful for calibrating
    //

    //! Set the number of frequency channels in the response array
    virtual void set_response_nchan (unsigned nchan);
    //! Get the number of frequency channels in the response array
    virtual unsigned get_response_nchan () const;

    //! Return the system response for the specified channel
    virtual Jones<float> get_response (unsigned ichan) const;

    //! Return true if the Receiver is set
    bool has_Receiver () const;

    //! Return the Receiver
    const Receiver* get_Receiver () const;

    //! Set the Receiver extension to that of the input Archive
    void set_Receiver (const Archive*);
    std::string get_receiver_basis_filename () const;


    // ///////////////////////////////////////////////////////////////////
    //
    // useful for unloading
    //

    //! Return true if the transformation for the specified channel is valid
    bool get_transformation_valid (unsigned ch) const;

    //! Set the transformation invalid flag for the specified channel
    void set_transformation_invalid (unsigned ch);

    //! Return the transformation for the specified channel
    const MEAL::Complex2* get_transformation (unsigned ichan) const;

    //! Return the transformation for the specified channel
    MEAL::Complex2* get_transformation (unsigned ichan);

    //! Return true if parameter covariances are stored
    bool has_covariance () const;

    //! Return the covariance matrix vector for the specified channel
    void get_covariance (unsigned ichan, std::vector<double>&) const;

    //! Return true if least squares minimization solvers are available
    virtual bool has_solver () const;

    //! Return the transformation for the specified channel
    virtual const MEAL::LeastSquares* get_solver (unsigned ichan) const;
    MEAL::LeastSquares* get_solver (unsigned ichan);

    // ///////////////////////////////////////////////////////////////////
    //
    // Pulsar::Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Perform backend corrections before inverting response
    void set_backend_correction (bool f) { do_backend_correction = f; }

    //! Calibrate the polarization of the given archive
    virtual void calibrate (Archive* archive);

    //! Return the Calibrator::Type of derived class
    virtual const Type* get_type () const;

    //! Get the number of frequency channels in the calibrator
    virtual unsigned get_nchan () const;

    //! Return a new PolnCalibratorExtension
    CalibratorExtension* new_Extension () const;

    //! Return plotting information
    Calibrator::Info* get_Info () const;

    //! Communicates PolnCalibrator parameters to plotting routines
    class Info;

    //! Set up done before calibrating an archive
    void calibration_setup (const Archive* arch);

  protected:

    //! The array of transformation Model instances
    Reference::Vector<MEAL::Complex2> transformation;

    //! The array of covariance matrix vectors
    std::vector< std::vector<double> > covariance;

    //! Set up to call calculate_transformation
    void setup_transformation () const;

    //! The array of Jones matrices derived from the transformation array
    std::vector< Jones<float> > response;

    //! The PolnCalibratorExtension of the Archive passed during construction
    Reference::To<const PolnCalibratorExtension> poln_extension;

    //! The FeedExtension of the Archive passed during construction
    Reference::To<const FeedExtension> feed;

    //! Flag set when response has been built
    bool built;

    //! The number of frequency channels in the observation to be calibrated
    unsigned observation_nchan;

    //! Perform backend corrections before inverting
    bool do_backend_correction;

    //! Derived classes can create and fill the transformation array
    virtual void calculate_transformation ();

    //! Derived classes can add conditions for channel validity
    virtual bool get_valid (unsigned ichan) const;

    //! Return the weight (0 or 1) of the specified channel
    float get_weight (unsigned ichan) const;

    //! Derived classes may be able to shrink the transformation array
    virtual unsigned get_maximum_nchan () const;

    //! The sub-integration from which to construct a solution
    Index subint;
    
  private:

    //! Build the response array
    void build (unsigned nchan = 0);
    
    //! Temporary storage space of solver statistics
    mutable Reference::Vector<MEAL::LeastSquares> tmp_solver;

    //! Temporary storage of bad channel flags
    std::vector<bool> bad;
    unsigned bad_count;

    void build_response();
    void patch_response();

    //! The Receiver Extension used as the basis for corrections
    Reference::To<const Receiver> receiver;
    std::string receiver_basis_filename;

    //! Used by constructors to initialize variables
    void init ();

  };

  //! Create a new transformation instance described by the extension
  MEAL::Complex2*
  new_transformation (const PolnCalibratorExtension*, unsigned ichan);

  //! Create a new transformation instance described by the type
  MEAL::Complex2* new_transformation( const Calibrator::Type* type );

  //! Create a new transformation instance based on the Calibrator::Type name
  MEAL::Complex2* transformation_factory (const std::string& name);

  //! Load a new transformation instance from file
  MEAL::Complex2* load_transformation (const std::string& filename);

  //! Create a new Calibrator::Type instance according to the transformation
  Calibrator::Type* new_CalibratorType( const MEAL::Complex2* xform ); 


  class PolnCalibrator::Info : public Calibrator::Info
  {

  public:

    //! Factory returns a suitable instance
    static PolnCalibrator::Info* create (const PolnCalibrator* calibrator);
    
    //! Constructor
    Info (const PolnCalibrator* calibrator);
    
    //! Return the title
    std::string get_title () const;

    //! Return the number of frequency channels
    unsigned get_nchan () const;

    //! Return the number of parameter classes
    unsigned get_nclass () const;
    
    //! Return the name of the specified class
    std::string get_name (unsigned iclass) const;
    
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

}

#endif

