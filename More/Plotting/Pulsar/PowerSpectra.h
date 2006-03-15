//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/Plotting/Pulsar/PowerSpectra.h,v $
   $Revision: 1.1 $
   $Date: 2006/03/15 12:29:45 $
   $Author: straten $ */

#ifndef __Pulsar_PowerSpectra_h
#define __Pulsar_PowerSpectra_h

#include "Pulsar/FrequencyPlot.h"

namespace Pulsar {

  class Profile;

  //! Plots flux profiles
  class PowerSpectra : public FrequencyPlot {

  public:

    //! Default constructor
    PowerSpectra ();

    //! Perform any preprocessing steps
    void prepare (const Archive*);

    //! Get the default label for the y axis
    std::string get_ylabel (const Archive*);

    //! Derived classes must provide the profiles
    virtual void get_spectra (const Archive*) = 0;

    //! Set the sub-integration to plot (where applicable)
    void set_subint (unsigned _isubint) { isubint = _isubint; }
    unsigned get_subint () const { return isubint; }
    
    //! Set the frequency channel to plot (where applicable)
    void set_chan (unsigned _ichan) { ichan = _ichan; }
    unsigned get_chan () const { return ichan; }

    //! Set the polarization to plot
    void set_pol (unsigned _ipol) { ipol = _ipol; }
    unsigned get_pol () const { return ipol; }

    //! Draw the spectra
    void draw (const Archive*);

    //! Draw the spectrum
    void draw (const std::vector<float>&) const;

    //! Return the text interface
    TextInterface::Class* get_interface ();

  protected:

    std::vector< std::vector<float> > spectra;
    std::vector< int > plot_sci;
    std::vector< int > plot_sls;

    unsigned isubint;
    unsigned ichan;
    unsigned ipol;

    bool plot_lines;

  private:

    std::vector<float> frequencies;

  };

}

#endif
