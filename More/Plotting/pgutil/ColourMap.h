//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/Plotting/pgutil/ColourMap.h

#ifndef __ColourMap_H
#define __ColourMap_H

#include "Pulsar/Config.h"
#include "TextInterface.h"
#include <string>

namespace pgplot {

  //! Colour map manager
  class ColourMap : public Reference::Able {

  public:
    //! Available colour map names
    enum Name {
      GreyScale,
      Inverse,
      Heat,
      Cold,
      Plasma,
      Forest,
      AlienGlow,
      CubeHelix,
      Test
    };

    //! Default constructor
    ColourMap ();

    ColourMap (const Name _name);

    //! Set the colour map name
    void set_name (Name name);
    Name get_name () const { return name; }

    //! Set the contrast (0 to 1; normally 1)
    void set_contrast (float contrast);
    float get_contrast () const { return contrast; }

    //! Set the brightness (0 to 1; normally 5)
    void set_brightness (float brightness);
    float get_brightness () const { return brightness; }

    //! Set the image transfer function to logarithmic
    void set_logarithmic (bool log) { logarithmic = log; }
    bool get_logarithmic () const { return logarithmic; }

    //! Apply the current attributes
    void apply ();

    //! The text interface to this class
    class Interface;
    
  protected:

    //! Colour map name
    Name name;

    //! Contrast
    float contrast;

    //! Brightness
    float brightness;

    //! Logarithmic image transfer function
    bool logarithmic;

  public:

    static Pulsar::Option<std::string> default_colour_map;
  };

  class ColourMap::Interface : public TextInterface::To<ColourMap> {
  public:
    Interface (ColourMap* = 0);
  };

  //! ColourMap::Name stream insertion operator
  std::ostream& operator << (std::ostream&, ColourMap::Name);

  //! ColourMap::Name stream extraction operator
  std::istream& operator >> (std::istream&, ColourMap::Name&);


}
#endif
