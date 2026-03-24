//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/ProjectionCorrection.h

#ifndef __Pulsar_ProjectionCorrection_H
#define __Pulsar_ProjectionCorrection_H

#include "Pulsar/Parallactic.h"
#include "Reference.h"
#include "Jones.h"

class MountProjection;

namespace Pulsar
{
  class Archive;
  class Receiver;
  class Telescope;
  class Integration;
  class Pointing;

  //! Computes the projection of the receptors onto the sky
  /*
    The projection consists of two parts:

    - J_antenna - projection from the sky to the antenna basis (e.g. Earth + mechanical rotation)
    - J_feed    - projection from the anntenna basis to the feed horn basis (e.g. feed rotation)

    If the Mount is of type MountProjection that computes its own projection transformation,
    then both J_antenna will be equal to the transformation returned by this object.
    Otherwise, J_antenna is a rotation about the line of sight by the parallactic angle.

    J_feed is a rotation about the line of sight by the feed angle.

    If one were to model the optics of the antenna (e.g. beam squint) then this
    transformation should be placed between J_antenna and J_feed; i.e.

    J_total = J_feed * J_optics * J_antenna

  */
  class ProjectionCorrection : public Reference::Able
  {
  public:

    //! Trust Pointing parallactic angle more than computed value
    static bool trust_pointing_para_angle;

    //! Always trust the feed angle stored in the Pointing extension
    static bool trust_pointing_feed_angle;

    //! Default constructor
    ProjectionCorrection ();
    //! Destructor
    ~ProjectionCorrection ();

    //! Set the archive for which projection corrections will be computed
    void set_archive (const Archive*);

    //! Return the projection correction for the given sub-integration
    /*! The Jones matrix should be inverted to calibrate observations. */
    Jones<double> operator () (unsigned isub) const;

    //! Return true if the given sub-integration requires correction
    bool required (unsigned isub) const;

    //! Return the projection from the antenna to the feed
    /* \pre The required method must be called before calling this method*/
    Jones<double> get_feed_projection () const;

    //! Return the projection from the sky to the antenna
    /* \pre The required method must be called before calling this method*/
    Jones<double> get_antenna_projection () const;

    //! Return the projection correction calculator
    Mount* get_mount ();
    
    //! Return a long summary of parameters relevant to computing the projection correction
    std::string get_summary () const;

    //! Return a short summary of parameters relevant to computing the projection correction
    std::string get_short_summary () const;

    //! Reset the summaries to empty strings
    void reset_summary () { summary = short_summary = ""; }

  protected:

    //! Return the antenna rotation through the parallactic angle
    /* \pre The required method must be called before calling this method*/
    Jones<double> get_antenna_rotation () const;

    //! Return the antenna projection computed by the Mount projection
    /* \pre The required method must be called before calling this method*/
    Jones<double> get_mount_projection () const;

    /** @name Correction Information
     *  These attributes are set during the call to required and
     *  are used by the correct method
     */
    //@{

    //! The archive for which projection corrections will be computed
    Reference::To<const Archive, false> archive;

    //! The telescope extension from the archive
    Reference::To<const Telescope, false> telescope;
    
    //! The receiver extension from the archive
    Reference::To<const Receiver, false> receiver;

    //! The sub-integration for which projection corrections will be computed
    mutable Reference::To<const Integration, false> integration;

    //! The pointing extension from the integration
    mutable Reference::To<const Pointing, false> pointing;

    //! The parallactic (vertical) angle rotation
    mutable Calibration::Parallactic para;
    
    //! Flag set when the receiver vertical angle should be corrected
    mutable bool should_correct_vertical;

    //! The projection correction calculator
    mutable Reference::To<MountProjection> projection;

    //! The type of mount
    mutable Reference::To<Mount> mount;

    //! Flag set when the receiver projection should be corrected
    mutable bool should_correct_projection;

    //! Flag set when either of the above corrections have not been performed
    mutable bool must_correct_platform;

    //! Summary of relevant parameters
    mutable std::string summary;

    //! Short summary of relevant parameters
    mutable std::string short_summary;
    //@}

  };
}

#endif
