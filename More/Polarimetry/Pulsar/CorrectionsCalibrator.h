//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Polarimetry/Pulsar/Attic/CorrectionsCalibrator.h,v $
   $Revision: 1.2 $
   $Date: 2004/07/20 07:38:39 $
   $Author: straten $ */

#ifndef __Pulsar_CorrectionsCalibrator_H
#define __Pulsar_CorrectionsCalibrator_H

#include "Pulsar/Calibrator.h"
#include "MJD.h"

namespace Pulsar {

  class Telescope;
  class Receiver;
  class Pointing;

  //! Corrects known instrumental effects
  class CorrectionsCalibrator : public Calibrator {

  public:

    //! The ReceptionCalibrator class makes use of protected attributes
    friend class ReceptionCalibrator;

    //! Default constructor
    CorrectionsCalibrator ();

    //! Destructor
    ~CorrectionsCalibrator ();

    //! Return true if the archive needs to be corrected
    bool needs_correction (const Archive* archive, const Pointing* point=0);

    //! Return the correction matrix for the given epoch
    Jones<double> get_transformation (const Archive* arch, unsigned isub);

    // ///////////////////////////////////////////////////////////////////
    //
    // Calibrator implementation
    //
    // ///////////////////////////////////////////////////////////////////

    //! Calibrate the Pulsar::Archive
    void calibrate (Archive* archive);

    //! Return the Calibrator::Type of this class
    Type get_type () const { return Corrections; }

    //! Get the number of frequency channels in the calibrator
    unsigned get_nchan () const { return 0; }


  protected:

    /** @name Correction Information
     *  These attributes are set during the call to needs_correction and
     *  are used by the correct method
     */
    //@{

    //! The Telescope Extension of the Archive to be corrected
    Reference::To<Telescope, false> telescope;

    //! The Receiver Extension of the Archive to be corrected
    Reference::To<Receiver, false> receiver;

    //! Flag set when the receiver vertical angle should be corrected
    bool should_correct_vertical;
    //! Flag set when the receiver projection should be corrected
    bool should_correct_projection;
    //! Flag set when either of the above corrections must be performed
    bool must_correct_platform;

    //! Flag set when the receptor offsets need to be corrected
    bool should_correct_receptors;
    //! Flag set when the calibrator offset needs to be corrected
    bool should_correct_calibrator;
    //! Flag set when either of the above corrections must be performed
    bool must_correct_feed;

    //@}

  };

}

#endif
