//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Extensions/Pulsar/Passband.h,v $
   $Revision: 1.2 $
   $Date: 2003/06/16 21:24:49 $
   $Author: straten $ */

#ifndef __Passband_h
#define __Passband_h

#include "Pulsar/Archive.h"

namespace Pulsar {

  //! Example Pulsar::Archive Extension
  /*! This Passband class provides an example of how to
    inherit the Pulsar::Archive::Extension class. By copying the files
    Passband.h and Passband.C and performing simple
    text-substitution, the skeleton of a new Extension may be easily
    developed. */
  class Passband : public Pulsar::Archive::Extension {

  public:
    
    //! Default constructor
    Passband ();

    //! Copy constructor
    Passband (const Passband& extension);

    //! Operator =
    const Passband& operator= (const Passband& extension);

    //! Get the number of frequency channels in each passband
    unsigned get_nchan () const;

    //! Get the number of polarizations
    unsigned get_npol () const;

    //! Get the number of bands
    unsigned get_nband () const;

    //! Set the number of channels, polarizations, and bands
    void resize (unsigned nchan, unsigned npol, unsigned nband=1);

    //! Get the specified passband
    const vector<float>& get_passband (unsigned ipol, unsigned iband=0) const;

    //! Set the specified passband
    void set_passband (const vector<float>&, unsigned ipol, unsigned iband=0);

    //! Set the specified passband
    void set_passband (const float* data, unsigned ipol, unsigned iband=0);

    //! Set all passband datum (not dimensions) to zero
    void zero ();

  protected:

    //! Number of frequency channels
    unsigned nchan;

    //! Number of polarizations
    /*! A full polarimetric representation of the bandpass may be stored. */
    unsigned npol;

    //! Number of frequency bands
    /*! The number of frequency bands may be other than one when an Archive
      contains data from multiple bands. */
    unsigned nband;

    //! Average passbands
    /*! The passbands are organized in ipol major order, I think.
      band0,pol0 band0,pol2 ... bandN,polM band1,pol0 ... */
    vector< vector<float> > passband;

    //! Throw an exception if ipol or iband are out of range
    void range_check (unsigned ipol, unsigned iband, const char* method) const;

  };
 
}

#endif
