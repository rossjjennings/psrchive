//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/tempo2/T2Observatory.h,v $
   $Revision: 1.1 $
   $Date: 2008/05/26 12:04:06 $
   $Author: straten $ */

#ifndef __Tempo2_Observatory_h
#define __Tempo2_Observatory_h

#include "Pulsar/Site.h"

namespace Tempo2
{

  //! Observatory data available from TEMPO
  class Observatory : public Pulsar::Site
  {
  public:

    //! Get the observatory name
    std::string get_name () const;
    void set_name (const std::string&);

    //! Get the geocentric XYZ coordinates in metres
    void get_xyz (double& x, double& y, double& z) const;
    void set_xyz (double x, double y, double z);

    //! Get the latitude and longitude in radians, radius in metres
    void get_sph (double& lat, double& lon, double& rad) const;

    //! Get the "old-style" telescope ID code
    std::string get_old_code () const;
    void set_old_code (const std::string&);

  protected:

    std::string name;
    std::string old_code;
    double x, y, z;
  };

  const Observatory* observatory (const std::string& telescope_name);

}

#endif

