//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/StokesSpherical.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/10 21:50:07 $
   $Author: straten $ */

#ifndef __Pulsar_StokesSpherical_h
#define __Pulsar_StokesSpherical_h

#include "Pulsar/MultiProfile.h"
#include "Pulsar/StokesPlotter.h"
#include "Pulsar/PosAngPlotter.h"
#include "Pulsar/EllAngPlotter.h"

namespace Pulsar {

  class PolnProfile;

  //! Plots a single pulse profile
  class StokesSpherical : public MultiProfile {

  public:

    //! Default constructor
    StokesSpherical ();

    //! Get the text interface to the configuration attributes
    TextInterface::Class* get_interface ();

    //! Ensure that frames are properly initialized
    void prepare (const Archive*);

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }

    //! Get the StokesPlotter
    StokesPlotter* get_flux () { return &flux; }

    //! Get the PosAngPlotter
    PosAngPlotter* get_orientation () { return &orientation; }

    //! Get the EllAngPlotter
    EllAngPlotter* get_ellipticity () { return &ellipticity; }

  protected:

    StokesPlotter flux;
    PosAngPlotter orientation;
    EllAngPlotter ellipticity;
    unsigned isubint;
    unsigned ichan;

    template<class T> void prepare (T* plot)
    {
      plot->set_subint (isubint);
      plot->set_chan (ichan);
    }

  };

}

#endif
