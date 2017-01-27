//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Util/tempo2/T2Observatory.h

#ifndef __Tempo2_Observatory_h
#define __Tempo2_Observatory_h

#include "Pulsar/Site.h"
#include <vector>

namespace Tempo2
{

  //! Observatory data available from TEMPO
  class Observatory : public Pulsar::Site
  {
  public:

    //! Default constructor
    Observatory ();

    bool is_alias_of(const std::string& name) const;
    inline bool is_alias_of(const Observatory &obs) const{
        return this->is_alias_of(obs.get_name());
    }
    inline bool is_alias_of(const char c) const{
        return this->is_alias_of(std::string(1,c));
    }

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

    //! Get the observatory number (from $TEMPO2/observatory/aliases)
    char get_code () const;
    void set_code (char);

    std::vector<std::string> aliases;
  protected:

    std::string name;
    std::string old_code;
    char code;
    double x, y, z;

  };

  const Observatory* observatory (const std::string& telescope_name);

}

#endif

