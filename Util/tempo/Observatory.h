//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo/Observatory.h,v $
   $Revision: 1.2 $
   $Date: 2008/05/23 12:37:48 $
   $Author: straten $ */

#ifndef __Tempo_Observatory_h
#define __Tempo_Observatory_h

#include "Reference.h"

namespace Tempo
{

  //! Observatory data available from TEMPO
  class Observatory : public Reference::Able
  {
  public:

    //! Default constructor
    Observatory ();

    //! Get the one-letter tempo code
    char get_code () const;
    void set_code (char);

    //! Get the two-letter ITOA code
    std::string get_itoa_code () const;
    void set_itoa_code (const std::string&);

    //! Get the observatory name
    std::string get_name () const;
    void set_name (const std::string&);

    //! Get the geocentric XYZ coordinates in metres
    virtual void get_xyz (double& x, double& y, double& z) const = 0;

    //! Get the latitude and longitude in radians
    /*! elevation is not implemented */
    virtual void get_latlonel (double& lat, double& lon, double& el) const = 0;

  protected:

    char code;
    std::string itoa_code;
    std::string name;

  };

  class ObservatoryITRF : public Observatory
  {
  public:

    //! Default constructor
    ObservatoryITRF (double x, double y, double z);

    //! Get the geocentric XYZ coordinates in metres
    virtual void get_xyz (double& x, double& y, double& z) const;

    //! Get the latitude and longitude in radians
    virtual void get_latlonel (double& lat, double& lon, double& el) const;

  protected:
    double x, y, z;
  };

  class ObservatoryWGS84 : public Observatory
  {
  public:

    //! Default constructor
    ObservatoryWGS84 (double lat, double lon, double el);

    //! Get the geocentric XYZ coordinates in metres
    virtual void get_xyz (double& x, double& y, double& z) const;

    //! Get the latitude and longitude in radians
    virtual void get_latlonel (double& lat, double& lon, double& el) const;

  protected:
    double lat, lon, el;
  };

}


#endif
