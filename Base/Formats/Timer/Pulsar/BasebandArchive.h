//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/BasebandArchive.h,v $
   $Revision: 1.3 $
   $Date: 2003/03/08 11:24:46 $
   $Author: straten $ */

#ifndef __Baseband_Archive_h
#define __Baseband_Archive_h

#include "TimerArchive.h"
#include "baseband_header.h"

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

    class Agent : public Archive::Advocate<BasebandArchive> {

      public:
 
        Agent () { }
 
        //! Advocate the use of BasebandArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        string get_name () { return "BasebandArch"; }

        //! Return description of this plugin
        string get_description ();

    };

    //! Baseband data reduction specific header information
    baseband_header bhdr;

    //! Digitization histograms
    vector< vector<float> > dls_histogram;

    //! Average passbands
    vector< vector<float> > passband;
  
    //! Load BasebandArchive specific information
    void backend_load (FILE* fptr);

    //! Unload BasebandArchive specific information
    void backend_unload (FILE* fptr) const;

    //! Convert the endian of baseband_header
    void convert_hdr_Endian ();

    void init ();

  private:
    static vector<float> empty;
  };


}

#endif


