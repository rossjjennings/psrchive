//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2019 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __Pulsar_VariableProjectionCorrection_h
#define __Pulsar_VariableProjectionCorrection_h

#include "Pulsar/KnownVariableTransformation.h"
#include "Pulsar/ProjectionCorrection.h"

namespace Pulsar {

  //! Adapts a KnonwnVariableTransformation to a ProjectionCorrection
  class VariableProjectionCorrection : public KnownVariableTransformation
  {
    mutable ProjectionCorrection correction;
    mutable LabelledJones<double> feed_projection;
    mutable LabelledJones<double> antenna_projection;
    mutable LabelledJones<double> transformation;

    void build () const;

  public:

    //! Get the projection correction
    ProjectionCorrection* get_correction () { return &correction; }
    
    //! Get the feed rotation
    LabelledJones<double> get_feed_projection ();

    //! Get the antenna projection
    LabelledJones<double> get_antenna_projection ();

    //! Get the transformation
    LabelledJones<double> get_value () override;

    //! Return true if the transformation is required
    bool required () const override;

    //! Return the description
    std::string get_description () const override;
  };
}

#endif
