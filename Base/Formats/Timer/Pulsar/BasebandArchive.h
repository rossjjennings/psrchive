//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/BasebandArchive.h,v $
   $Revision: 1.6 $
   $Date: 2003/08/14 19:02:37 $
   $Author: straten $ */

#ifndef __Baseband_Archive_h
#define __Baseband_Archive_h

#include "TimerArchive.h"
#include "baseband_header.h"

#include "Pulsar/Passband.h"
#include "Pulsar/dspReduction.h"
#include "Pulsar/TwoBitStats.h"

namespace Pulsar {

  //! Pulsar Archive produced by psrdisp
  class BasebandArchive : public TimerArchive {

  public:

    //! null constructor
    BasebandArchive ();

    //! copy constructor
    BasebandArchive (const Archive& archive);

    //! copy constructor
    BasebandArchive (const BasebandArchive& archive);

    //! destructor
    ~BasebandArchive ();

    //! Copy the contents of an Archive into self
    virtual void copy (const Archive& archive);

    //! Returns a pointer to a new copy of self
    virtual Archive* clone () const;

    // //////////////////////////////////////////////////////////////////
    //
    // Extension access
    //
    // //////////////////////////////////////////////////////////////////

    //! Return the number of extensions available
    unsigned get_nextension () const;

    //! Return a pointer to the specified extension
    const Extension* get_extension (unsigned iextension) const;

    //! Return the number of passbands
    int get_npassband () const { return bhdr.pband_channels; };
    //! Return a reference to the bandpass in the given channel
    const vector<float>& get_passband (unsigned channel) const;

    //! Return the number of analog channels
    int get_nhistogram () const { return bhdr.analog_channels; };
    //! Return a reference to the digitization histogram in the given channel
    const vector<float>& get_histogram (unsigned channel) const;

    //! Set the hanning smoothing factor
    void set_hanning_smoothing_factor (int factor);

    //! Get the hanning smoothing factor
    int get_hanning_smoothing_factor () const;

    //! Get the tolerance (old - disused)
    float get_tolerance () const;

    //! Get the name of the apodizing function
    const char* get_apodizing_name () const;

    //! Return true if data was scattered power corrected
    bool get_scattered_power_corrected () const;

    //! Return true if phase-coherent polarimetric calibration was performed
    bool get_coherent_calibration () const;


  protected:

    friend class Archive::Advocate<BasebandArchive>;

    class Agent : public Archive::Advocate<BasebandArchive> {

      public:
 
        //! Advocate the use of BasebandArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        string get_name () { return "BasebandArch"; }

        //! Return description of this plugin
        string get_description ();

    };

    //! Baseband data reduction specific header information
    baseband_header bhdr;

    //! Baseband data reduction Archive::Extension
    dspReduction reduction;

    //! Two-bit sampling statistics
    TwoBitStats twobit;

    //! Average passbands in Archive::Extension
    Passband passband;
  
    //! Update the baseband_header struct with the current state
    void set_header ();

    //! Update the reduction attribute with the current state
    void set_reduction ();

    //! Load BasebandArchive specific information
    void backend_load (FILE* fptr);

    //! Unload BasebandArchive specific information
    void backend_unload (FILE* fptr) const;

    //! Convert the endian of baseband_header
    void convert_hdr_Endian ();

    //! Over-ride TimerArchive::correct
    void correct ();

    void init ();

  };


}

#endif


