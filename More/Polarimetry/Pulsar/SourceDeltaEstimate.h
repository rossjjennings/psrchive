//-*-C++-*-

/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Polarimetry/Pulsar/SourceDeltaEstimate.h

#ifndef __Pulsar_SourceDeltaEstimate_H
#define __Pulsar_SourceDeltaEstimate_H

#include "Pulsar/SourceEstimate.h"

namespace Calibration
{
  //! Manages a delta source and the current best estimate of the total
  class SourceDeltaEstimate : public SourceEstimate
  {
  public:

    //! Construct with the specified bin from Archive
    SourceDeltaEstimate (int ibin = -1) : SourceEstimate (ibin) {}

    //! Create the source and add it to the measurement equation
    void create_source (ReceptionModel* equation);

    //! Set the baseline to which the delta is added
    void set_baseline (SourceEstimate*);

    //! Return true if baseline is set
    bool has_baseline () const;

    //! Return true if total is set
    bool has_total () const;

  protected:

    Reference::To< MEAL::SumRule<MEAL::Complex2> > total;

    Reference::To<SourceEstimate> baseline;

    //! does the work for update method, which does some generic extra bits
    void update_work ();    
  };

}

#endif
