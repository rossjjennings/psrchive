//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/pgutil/ColourMap.h,v $
   $Revision: 1.2 $
   $Date: 2004/10/22 15:53:05 $
   $Author: straten $ */

#ifndef __ColourMap_H
#define __ColourMap_H

namespace pgplot {

  //! Convenience interface to a bunch of regularly used colour maps
  class ColourMap {

  public:

    //! Default constructor
    ColourMap ();

    //! Available colour map names
    enum Name {
      GreyScale,
      Inverse,
      Heat,
      Cold,
      Plasma,
      Forest,
      AlienGlow,
      Test
    };

    //! Set the colour map name
    void set_name (Name name);

    //! Set the contrast (0 to 1; normally 1)
    void set_contrast (float contrast);

    //! Set the brightness (0 to 1; normally 5)
    void set_brightness (float brightness);

  protected:

    //! Colour map name
    Name name;

    //! Contrast
    float contrast;

    //! Brightness
    float brightness;

  };

}

#endif
