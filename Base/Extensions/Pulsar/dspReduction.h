//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/dspReduction.h,v $
   $Revision: 1.2 $
   $Date: 2003/06/17 13:41:35 $
   $Author: straten $ */

#ifndef __dspReduction_h
#define __dspReduction_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Example Pulsar::Archive Extension
  /*! This dspReduction class provides an example of how to
    inherit the Pulsar::Archive::Extension class. By copying the files
    dspReduction.h and dspReduction.C and performing simple
    text-substitution, the skeleton of a new Extension may be easily
    developed. */
  class dspReduction : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    dspReduction ();

    //! Copy constructor
    dspReduction (const dspReduction& extension);

    //! Operator =
    const dspReduction& operator= (const dspReduction& extension);

    //! Destructor
    ~dspReduction ();

    /** @name dsp::Input parameters
     *  These parameters describe the state of the dsp::Input from which the
     *  raw data was loaded
     */
    //@{

    //! The total number of time samples
    uint64 get_total_samples () const { return total_samples; }
    void set_total_samples (uint64 _total_samples)
    { total_samples= _total_samples; }

    //! The number of time samples in each block
    uint64 get_block_size () const { return block_size; }
    void set_block_size (uint64 _block_size)
    { block_size = _block_size; }

    //! The number of time samples by which consecutive blocks overlap
    uint64 get_overlap () const { return overlap; }
    void set_overlap (uint64 _overlap)
    { overlap = _overlap; }

    //! The state of the signal from Input::info
    Signal::State get_state () const { return state; }
    void set_state (Signal::State _state)
    { state = _state; }

    //@}

    /** @name dsp::Convolution parameters 
     *  These parameters describe the state of the
     *  dsp::Convolution instance used to process the data.
     */
    //@{

    //! Number of time samples in each FFT
    unsigned get_nsamp_fft () const { return nsamp_fft; }
    void set_nsamp_fft (unsigned _nsamp_fft)
    { nsamp_fft = _nsamp_fft; }

    //! Number of time samples in wrap-around region, left-hand side
    unsigned get_nsamp_overlap_pos () const { return nsamp_overlap_pos; }
    void set_nsamp_overlap_pos (unsigned _nsamp_overlap_pos)
    { nsamp_overlap_pos = _nsamp_overlap_pos; }

    //! Number of time samples in wrap-around region, right-hand side
    unsigned get_nsamp_overlap_neg () const { return nsamp_overlap_neg; }
    void set_nsamp_overlap_neg (unsigned _nsamp_overlap_neg)
    { nsamp_overlap_neg = _nsamp_overlap_neg; }

    //@}

    /** @name dsp::Filterbank parameters 
     *  These parameters describe the state of the
     *  dsp::Filterbank instance used to process the data.
     */
    //@{

    //! Number of frequency channels
    unsigned get_nchan () const { return nchan; }
    void set_nchan (unsigned _nchan)
    { nchan = _nchan; }

    //! Frequency resolution
    unsigned get_freq_res () const { return freq_res; }
    void set_freq_res (unsigned  _freq_res)
    {  freq_res= _freq_res; }

    //! Time resolution
    unsigned get_time_res () const { return time_res; }
    void set_time_res (unsigned _time_res)
    {  time_res= _time_res; }

    //@}


    /** @name dsp::TScrunch parameters 
     *  These parameters describe the state of the
     *  dsp::TScrunch instance used to process the data.
     */
    //@{

    unsigned get_ScrunchFactor () const { return ScrunchFactor; }
    void set_ScrunchFactor (unsigned _ScrunchFactor)
    { ScrunchFactor = _ScrunchFactor; }


    //@}

    /** @name dsp::PhaseSeries parameters 
     *  These parameters describe the state of the dsp::PhaseSeries
     *  instance from which the Pulsar::Profile data are constructed.
     */
    //@{

    //! Pulsar::Profiles are normalized by Observation::scale 
    double get_scale () const { return scale; }
    void set_scale (double _scale) 
    { scale = _scale; }

    //@}


  protected:

    /** @name dsp::Input parameters
     *  These parameters describe the state of the dsp::Input from which the
     *  raw data was loaded
     */
    //@{

    //! The total number of time samples
    uint64 total_samples;

    //! The number of time samples in each block
    uint64 block_size;

    //! The number of time samples by which consecutive blocks overlap
    uint64 overlap;

    //! The state of the signal from Input::info
    Signal::State state;

    //@}

    /** @name dsp::Convolution parameters 
     *  These parameters describe the state of the
     *  dsp::Convolution instance used to process the data.
     */
    //@{

    //! Number of time samples in each FFT
    unsigned nsamp_fft;

    //! Number of time samples in wrap-around region, left-hand side
    unsigned nsamp_overlap_pos;

    //! Number of time samples in wrap-around region, right-hand side
    unsigned nsamp_overlap_neg;

    //@}

    /** @name dsp::Filterbank parameters 
     *  These parameters describe the state of the
     *  dsp::Filterbank instance used to process the data.
     */
    //@{

    //! Number of frequency channels
    unsigned nchan;

    //! Frequency resolution
    unsigned freq_res;

    //! Time resolution
    unsigned time_res;

    //@}


    /** @name dsp::TScrunch parameters 
     *  These parameters describe the state of the
     *  dsp::TScrunch instance used to process the data.
     */
    //@{

    unsigned ScrunchFactor;

    //@}

    /** @name dsp::PhaseSeries parameters 
     *  These parameters describe the state of the dsp::PhaseSeries
     *  instance from which the Pulsar::Profile data are constructed.
     */
    //@{

    //! Pulsar::Profiles are normalized by Observation::scale 
    double scale;

    //@}

  };
 

}

#endif
