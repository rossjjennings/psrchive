//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Attic/AzZen.h,v $
   $Revision: 1.1 $
   $Date: 2008/07/02 10:30:05 $
   $Author: straten $ */

#ifndef __AzZen_H
#define __AzZen_H

#include "Horizon.h"

//! Calculates horizon pointing parameters using SLALIB

class AzZen : public Horizon
{  
public:
  
  //! Get the azimuth angle in radians
  double get_azimuth () const;

  //! Get the elevation angle in radians
  double get_elevation () const;

  //! Get the zenith angle in radians
  double get_zenith () const;

  //! Get the receptor basis in the reference frame of the observatory
  Matrix<3,3,double> get_basis (const Vector<3,double>& from_source) const;

protected:

  //! The telescope azimuth in radians
  mutable double azimuth;

  //! The telescope elevation in radians
  mutable double elevation;

};

#endif
