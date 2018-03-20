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

    //! Set sampler freq
    void set_fsamp(double f) { f_samp = f; }

    //! Get sampler freq
    double get_fsamp() const { return f_samp; }

    //! Set center freq
    void set_fcenter(double f) { f_cent = f; }

    //! Get center freq
    double get_fcenter() const { return f_cent; }

    //! Set band direction at sampler
    void set_band_dir(int d) { dir = d; }

    //! Get band direction
    int get_band_dir() const { return dir; }

    //! Set Nyquist zone (0-based) 
    void set_nyquist_zone(int z) { nyzone = z; }

    //! Get Nyquist zone
    int get_nyquist_zone() const { return nyzone; }

  protected:

    //! The sampling frequency (equal to BW for interleaved samplers)
    double f_samp;

    //! The center sky frequency of the sampled band
    double f_cent;

    //! The Nyquist zone in which the band is sampled (0-based)
    int nyzone;

    //! Band direction at the sampler
    int dir;

    // Note, all of the above imply that the conversion between
    // sky freq (f_sky) and sampled freq (f_adc) are:
    //   f_adc = dir*(f_sky-f_cent) + (nyzone+0.5)*f_samp
    double f_adc(double f_sky) const 
    { 
      return (double)dir*(f_sky-f_cent) + ((double)nyzone+0.5)*f_samp; 
    }

    //! strict==true means uncorrectable channels will be zero-weighted
    bool strict;

    //! Set true if data have been coherently dedispersed
    bool coherent;

    //! The time misalignment between samplers, per-poln (us)
    double dt[2];

    //! The gain mismatch between samplers, per-poln
    double alpha[2];

    //! The ratio of image power observed at sampler freq f
    //  to original power at freq f_samp - f
    double ratio(double f, unsigned ipol) const;

    //! Find channel matching given sky freq, returns -1 for no match
    int match_channel(const Integration *subint, double f_sky) const;

  };

}

#endif
