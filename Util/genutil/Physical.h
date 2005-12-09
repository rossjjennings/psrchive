//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/Physical.h,v $
   $Revision: 1.2 $
   $Date: 2005/12/09 16:39:54 $
   $Author: straten $ */

#ifndef __Pulsar_Physical_h
#define __Pulsar_Physical_h

namespace Pulsar {

  //! Speed of light in m/s
  const double speed_of_light = 299792458;

  //! Returns the dispersion delay between a frequency and reference frequency
  double dispersion_delay (double dm, double reference_freq, double freq);

}

#endif
