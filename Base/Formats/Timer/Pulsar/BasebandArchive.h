//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/BasebandArchive.h,v $
   $Revision: 1.12 $
   $Date: 2004/08/13 11:56:02 $
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

    //! Default constructor
    BasebandArchive ();

    //! Copy constructor
    BasebandArchive (const BasebandArchive& archive);

    //! Destructor
    ~BasebandArchive ();

    //! Assignment operator
    const BasebandArchive& operator = (const BasebandArchive& archive);
    
    //! Base copy constructor
    BasebandArchive (const Archive& archive);

    //! Base extraction constructor
    BasebandArchive (const Archive& archive, const vector<unsigned>& subint);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive, const vector<unsigned>& subints);

    //! Return a pointer to a new copy constructed instance equal to this
    virtual BasebandArchive* clone () const;

    //! Return a pointer to a new extraction constructed instance equal to this
    virtual BasebandArchive* extract (const vector<unsigned>& subints) const;

    // //////////////////////////////////////////////////////////////////
    //
    // BasebandArchive specific
    //

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

    //! Represents the interests of the BasebandArchive
    class Agent : public Archive::Advocate<BasebandArchive> {

      public:

        //! Default constructor (necessary even when empty)
        Agent () {}

        //! Advocate the use of BasebandArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the BasebandArchive plugin
        string get_name () { return "Baseband"; }

        //! Return description of this plugin
        string get_description ();

    };

    //! Baseband data reduction specific header information
    baseband_header bhdr;

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

    //! Set the backend data size information in the timer header
    void set_be_data_size ();

    //! Check the backend data size information without reseting header
    void check_be_data_size ();

    //! Check the Passband and TwoBitStats Extensions
    void check_extensions ();

    void init ();

    //! Correct memory alignment error in pre-Version 5 headers
    void fix_header_memory_alignment ();

  };


}

#endif


