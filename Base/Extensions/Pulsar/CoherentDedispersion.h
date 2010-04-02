//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2010 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/CoherentDedispersion.h,v $
   $Revision: 1.1 $
   $Date: 2010/04/02 10:27:44 $
   $Author: straten $ */

#ifndef __CoherentDedispersion_h
#define __CoherentDedispersion_h

#include "Pulsar/ArchiveExtension.h"

namespace Pulsar {

  //! Stores dspsr data reduction parameters
  /*! This class stores backend information specific to the baseband
    recording and processing system implemented by dspsr.
   */
  class CoherentDedispersion : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    CoherentDedispersion ();

    //! Clone method
    CoherentDedispersion* clone () const;

    //! get the text interface 
    TextInterface::Parser* get_interface();

    // Text interface to a CoherentDedispersion instance
    class Interface : public TextInterface::To<CoherentDedispersion>
    {
      public:
	Interface( CoherentDedispersion* = NULL );
    };

    //! Return a short name
    std::string get_short_name () const { return "cd"; }

    //////////////////////////////////////////////////////////////////////
    //
    // implementation
    //
    //////////////////////////////////////////////////////////////////////

    //! Domain in which the algorithm operates (time or frequency)
    Signal::Dimension get_domain () const { return domain; }
    void set_domain (Signal::Dimension val) { domain = val; }

    //! Description of the algorithm
    std::string get_descriptor () const { return descriptor; }
    void set_descriptor (const std::string& val) { descriptor = val; }

    //! Dispersion measure used for coherent dedispersion
    double get_dispersion_measure () const { return dispersion_measure; }
    void set_dispersion_measure (double val) { dispersion_measure = val; }

    //! Doppler shift correction applied to frequencies and bandwidths
    double get_doppler_correction () const { return doppler_correction; }
    void set_doppler_correction (double val) { doppler_correction = val; }

    //! Number of frequency channels in the input signal
    unsigned get_nchan_input () const { return nchan_input; }
    void set_nchan_input (unsigned nchan) { nchan_input = nchan; }

    //! Number of frequency channels in the output signal
    unsigned get_nchan_output () const { return nchan_output; }
    void set_nchan_output (unsigned nchan) { nchan_output = nchan; }

    //! Number of quantization levels for data
    unsigned get_nlev_data () const { return nlev_data; }
    void set_nlev_data (unsigned nlev) { nlev_data = nlev; }

    //! Number of quantization levels for chirp
    unsigned get_nlev_chirp () const { return nlev_chirp; }
    void set_nlev_chirp (unsigned nlev) { nlev_chirp = nlev; }

    //! Number of complex time samples in each cyclical convolution
    unsigned get_nsamp (unsigned ichan_out) const;
    //! Number of complex time samples in each cyclical convolution
    void set_nsamp (unsigned ichan_out, unsigned);

    //! Number of complex time samples in wrap-around region, left-hand side
    unsigned get_nsamp_overlap_pos (unsigned ichan_out) const;
    //! Number of complex time samples in wrap-around region, left-hand side
    void set_nsamp_overlap_pos (unsigned ichan_out, unsigned);

    //! Number of complex time samples in wrap-around region, right-hand side
    unsigned get_nsamp_overlap_neg (unsigned ichan_out) const;
    //! Number of complex time samples in wrap-around region, right-hand side
    void set_nsamp_overlap_neg (unsigned ichan_out, unsigned);

    //! Centre frequency of input channel
    double get_centre_frequency_input (unsigned ichan_out) const;
    //! Centre frequency of input channel
    void set_centre_frequency_input (unsigned ichan_out, double);

    //! Centre frequency of input channel
    double get_bandwidth_input (unsigned ichan_out) const;
    //! Centre frequency of input channel
    void set_bandwidth_input (unsigned ichan_out, double);

    //! Centre frequency of output channel
    double get_centre_frequency_output (unsigned ichan_out) const;
    //! Centre frequency of output channel
    void set_centre_frequency_output (unsigned ichan_out, double);

    //! Centre frequency of output channel
    double get_bandwidth_output (unsigned ichan_out) const;
    //! Centre frequency of output channel
    void set_bandwidth_output (unsigned ichan_out, double);

  protected:

    //! Domain in which the algorithm operates (time or frequency)
    Signal::Dimension domain;

    //! Description of the algorithm
    std::string descriptor;

    //! Dispersion measure used for coherent dedispersion
    double dispersion_measure;

    //! Doppler shift correction applied to frequencies and bandwidths
    double doppler_correction;

    //! Number of frequency channels in the input signal
    unsigned nchan_input;

    //! Number of frequency channels in the output signal
    unsigned nchan_output;

    //! Number of quantization levels for data
    unsigned nlev_data;

    //! Number of quantization levels for chirp
    unsigned nlev_chirp;

    //! Number of complex time samples in each cyclical convolution
    std::vector<unsigned> nsamp;

    //! Number of complex time samples in wrap-around region, left-hand side
    std::vector<unsigned> nsamp_overlap_pos;

    //! Number of complex time samples in wrap-around region, right-hand side
    std::vector<unsigned> nsamp_overlap_neg;

    //! Centre frequency of input channel
    std::vector<double> centre_frequency_input;

    //! Centre frequency of input channel
    std::vector<double> bandwidth_input;

    //! Centre frequency of output channel
    std::vector<double> centre_frequency_output;

    //! Centre frequency of output channel
    std::vector<double> bandwidth_output;

    void check_index_out (unsigned i, const char* method) const;
    void check_index_in (unsigned i, const char* method) const;

  };

}

#endif
