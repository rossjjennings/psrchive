//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 - 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/Smooth.h

#ifndef __Pulsar_Smooth_h
#define __Pulsar_Smooth_h

#include "Pulsar/Transformation.h"
#include "TextInterface.h"
#include "PhaseRange.h"

namespace Pulsar {

  class Profile;

  //! Profile smoothing algorithms
  class Smooth : public Transformation<Profile>
  {

  public:

    //! Default constructor
    Smooth ();

    //! Destructor
    virtual ~Smooth ();

    //! Return a copy constructed instance of self
    virtual Smooth* clone () const = 0;

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Text-based interface to Smooth class properties
    class Interface;

    //! Set the width of the smoothing function
    void set_width (const Phase::Value& w) { width = w; }

    //! Get the width of the smoothing function
    Phase::Value get_width () const { return width; }

    //! Set the width of the smoothing window in turns
    void set_turns (float);

    //! Get the width of the smoothing window in turns
    float get_turns () const;

    //! Set the width of the smoothing window in phase bins
    void set_bins (float);

    //! Get the width of the smoothing window in phase bins
    float get_bins () const;

    //! Construct a new Smooth instance from a string
    static Smooth* factory (const std::string& name_and_options);

  protected:

    //! Get the width of the smoothing window in phase bins
    float get_bins (const Profile*);

    //! Get the width of the smoothing window in turns
    float get_turns (const Profile*);

    //! The width of the smoothing 
    Phase::Value width;
  };

  //! 
  class Smooth::Interface : public TextInterface::To<Smooth>
  {
  public:
    Interface (Smooth* instance);
  };
}

#endif
