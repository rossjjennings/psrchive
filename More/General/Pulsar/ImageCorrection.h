//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ImageCorrection.h

#ifndef __Pulsar_ImageCorrection_h
#define __Pulsar_ImageCorrection_h

#include "Pulsar/Transformation.h"

#include "TextInterface.h"

namespace Pulsar {

  class Archive;
  class Integration;

  //! Corrects band-reversed signal due to poor image rejection
  class ImageCorrection : public Transformation<Integration> {

  public:

    //! Default constructor
    ImageCorrection ();

    //! Perform correction on full archive
    void correct (Archive*);

    //! Perform correction on one integration
    void transform (Integration*);

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface ();

    //! Text-based interface to class properties
    class Interface;

    //! Set strict mode
    void set_strict(bool flag=true) { strict = flag; }

    //! Get strict setting
    bool get_strict() const { return strict; }

    //! Set coherent dedisp flag
    void set_coherent(bool flag=true) { coherent = flag; }

    //! Get coherent dedisp flag
    bool get_coherent() const { return coherent; }

    //! Set center freq at sampler
    void set_fadc(double f) { f_cent_adc = f; }

    //! Set center freq at sampler
    double get_fadc() const { return f_cent_adc; }

    //! Set sky center freq
    void set_fcenter(double f) { f_cent_sky = f; }

    //! Get sky center freq
    double get_fcenter() const { return f_cent_sky; }

    //! Set band direction at sampler
    void set_band_dir(int d) { dir = d; }

    //! Get band direction
    int get_band_dir() const { return dir; }

    //! Set dt per poln
    void set_dt(double dt0, double dt1) { dt[0]=dt0; dt[1]=dt1; }

    //! Set gain per poln
    void set_gain(double a0, double a1) { alpha[0]=a0; alpha[1]=a1; }

  protected:

    //! The center frequency of the band at the ADC (MHz)
    double f_cent_adc;

    //! The center sky frequency of the sampled band (MHz)
    double f_cent_sky;

    //! Band direction at the sampler
    int dir;

    // Note, all of the above imply that the conversion between
    // sky freq (f_sky) and sampled freq (f_adc) are:
    //   f_adc = dir*(f_sky-f_cent_sky) + f_cent_adc
    double f_adc(double f_sky) const 
    { 
      return (double)dir*(f_sky-f_cent_sky) + f_cent_adc;
    }

    //! strict==true means uncorrectable channels will be zero-weighted
    bool strict;

    //! Set true if data have been coherently dedispersed
    bool coherent;

    //! The time misalignment between samplers, per-poln (us)
    double dt[2];

    //! The gain mismatch between samplers, per-poln
    double alpha[2];

    //! The ratio of image power from input sampler freq f
    //  into channel at 2*f_center - f
    double ratio(double f, unsigned ipol) const;

    //! Find channel matching given sky freq, returns -1 for no match
    int match_channel(const Integration *subint, double f_sky) const;

  };

}

#endif
