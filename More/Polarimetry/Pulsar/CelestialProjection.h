//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2025 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/CelestialProjection.h

#ifndef __Calibration_CelestialProjection_H
#define __Calibration_CelestialProjection_H

#include "MEAL/Complex2.h"
#include "MountProjection.h"

namespace Calibration {

  //! Projection of the receptors onto the celestial sphere
  class CelestialProjection : public MEAL::Complex2
  {

  public:

    //! Default constructor
    CelestialProjection ();

    //! Get the name of the class
    std::string get_name () const override { return "CelestialProjection"; }

    //! Evaluate the projection
    void calculate (Jones<double>& result, std::vector<Jones<double>>* grad = nullptr) override;

    //! Set the projection antenna
    void set_projection (MountProjection*);

    //! Get the projection antenna
    MountProjection* get_projection ();

    //! Get the projection antenna
    const MountProjection* get_projection () const;

    //! Set the MJD
    void set_epoch (const MJD& epoch);

    //! Get the MJD
    MJD get_epoch () const;

    //! Set the hour angle in radians
    void set_hour_angle (double radians);

    //! Get the hour angle in radians
    double get_hour_angle () const;

  protected:

    Reference::To<MountProjection> projection;

  };

}

#endif
