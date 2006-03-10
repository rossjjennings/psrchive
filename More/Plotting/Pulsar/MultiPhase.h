//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/MultiPhase.h,v $
   $Revision: 1.2 $
   $Date: 2006/03/10 16:33:01 $
   $Author: straten $ */

#ifndef __Pulsar_MultiProfile_h
#define __Pulsar_MultiProfile_h

#include "Pulsar/ProfilePlotter.h"
#include "Pulsar/MultiFrame.h"

namespace Pulsar {

  //! Plots multiple viewports with pulse phase along the shared x-axis
  class MultiProfile : public ProfilePlotter {

  public:

    //! Default constructor
    MultiProfile ();

    //! Plot in the current viewport
    void plot (const Archive*);

    //! Manage a plotter
    void manage (const std::string& name, ProfilePlotter* plot);

    //! Set the viewport of the named plotter
    void set_viewport (const std::string& name,
		       std::pair<float,float> x_range,
		       std::pair<float,float> y_range);

  protected:

    //! The plot frames
    MultiFrame frames;

    //! The plotters
    std::map< std::string, Reference::To<ProfilePlotter> > plotters;

  };

}

#endif
