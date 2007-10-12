//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/Smooth.h,v $
   $Revision: 1.9 $
   $Date: 2007/10/12 02:45:59 $
   $Author: straten $ */

#ifndef __Pulsar_Smooth_h
#define __Pulsar_Smooth_h

#include "Pulsar/Transformation.h"

namespace Pulsar {

  class Profile;

  //! Profile smoothing algorithms
  class Smooth : public Transformation<Profile> {

  public:

    //! Default constructor
    Smooth ();

    //! Destructor
    virtual ~Smooth ();

    //! Set the width of the smoothing window in turns
    void set_turns (float);

    //! Get the width of the smoothing window in turns
    float get_turns () const;

    //! Set the width of the smoothing window in phase bins
    void set_bins (float);

    //! Get the width of the smoothing window in phase bins
    float get_bins () const;

  protected:

    //! Get the width of the smoothing window in phase bins
    float get_bins (const Profile*);

    //! Get the width of the smoothing window in turns
    float get_turns (const Profile*);

    //! The width of the bins used to smooth
    float turns;

    //! The number of phase bins in the bins used to smooth
    float bins;

  };

}

#endif
