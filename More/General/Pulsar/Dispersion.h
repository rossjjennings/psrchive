//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2006-2026 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Dispersion.h

#ifndef __Pulsar_Dispersion_h
#define __Pulsar_Dispersion_h

#include "Pulsar/ColdPlasma.h"
#include "Pulsar/Config.h"
#include "Pulsar/DispersionDelay.h"
#include "Pulsar/DispersionHistory.h"
#include "Pulsar/IntegrationBarycentre.h"

namespace Pulsar {

  //! Corrects dispersive delays
  /*! Rotates the phase of each profile in each frequency channel to
    remove dispersive delays with respect to the reference frequency.

    \post All profiles will be phase-aligned to the reference frequency
  */
  class Dispersion : public ColdPlasma<DispersionDelay,DispersionHistory> {

  public:

    //! Default constructor
    Dispersion ();

    //! Return the dispersion measure
    /*! As returned by psredit -c dm */
    double get_relative_measure (const Integration*) const override;

    //! Return the auxiliary dispersion measure
    /*! As returned by psredit -c int:aux:dm */
    double get_absolute_measure (const Integration*) const override;

    //! Return true if the dispersion measure has been corrected with respect to centre frequency
    /*! As returned by psredit -c dmc */
    bool get_relative_corrected (const Integration*) const override;

    //! Return true if the auxiliary dispersion measure has been corrected with respect to centre frequency
    /*! As returned by psredit -c aux:dmc */
    bool get_absolute_corrected (const Integration*) const override;

    //! Return zero delay
    double get_identity () const override { return 0; }

    //! Combine delays
    void combine (double& result, const double& add) const override { result += add; }

    //! Setup all necessary attributes
    void update (const Integration*) override;

    //! Phase rotate each profile by the correction
    void apply (Integration*, unsigned channel, double delay) override;

    //! Apply the current correction to all sub-integrations in an archive
    void execute (Archive*) override;

    //! Undo the correction
    void revert (Archive*) override;

    //! Set the dispersion measure
    void set_dispersion_measure (double dispersion_measure)
    { relative.set_measure (dispersion_measure); }
      
    //! Get the dispersion measure
    double get_dispersion_measure () const
    { return relative.get_measure (); }

    //! Get the dispersive delay in seconds
    double get_delay () const;

    //! Get the dispersive phase shift in turns
    double get_shift () const;

  protected:

    double folding_period;

    static Option<bool> barycentric_correction;
    IntegrationBarycentre bary;
    double earth_doppler;

  };

}

#endif
