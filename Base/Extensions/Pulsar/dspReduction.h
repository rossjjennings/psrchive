//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/dspReduction.h,v $
   $Revision: 1.1 $
   $Date: 2003/06/17 10:45:01 $
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
