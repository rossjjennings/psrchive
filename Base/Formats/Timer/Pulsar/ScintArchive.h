//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/Timer/Pulsar/ScintArchive.h,v $
   $Revision: 1.1 $
   $Date: 2005/02/17 10:07:27 $
   $Author: straten $ */

#ifndef __Scint_Archive_h
#define __Scint_Archive_h

#include "TimerArchive.h"
#include "baseband_header.h"

#include "Pulsar/Passband.h"
#include "Pulsar/dspReduction.h"
#include "Pulsar/TwoBitStats.h"

namespace Pulsar {

  //! Pulsar Archive produced by psrdisp
  class ScintArchive : public TimerArchive {

  public:

    //! Default constructor
    ScintArchive ();

    //! Copy constructor
    ScintArchive (const ScintArchive& archive);

    //! Destructor
    ~ScintArchive ();

    //! Assignment operator
    const ScintArchive& operator = (const ScintArchive& archive);
    
    //! Base extraction constructor
    ScintArchive (const Archive& archive, const vector<unsigned>& subint);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive, const vector<unsigned>& subints);

    //! Return a pointer to a new copy constructed instance equal to this
    virtual ScintArchive* clone () const;

    //! Return a pointer to a new extraction constructed instance equal to this
    virtual ScintArchive* extract (const vector<unsigned>& subints) const;

    // //////////////////////////////////////////////////////////////////
    //
    // ScintArchive specific
    //

    //! Return a reference to the bandpass in the given channel
    const vector<float>& get_passband (unsigned channel) const;

    bool takes (const ScintArchive* archive) const;

    void integrate (const ScintArchive* archive);

  protected:

    vector< vector<float> > spectra;

    friend class Archive::Advocate<ScintArchive>;

    //! Represents the interests of the ScintArchive
    class Agent : public Archive::Advocate<ScintArchive> {

      public:

        //! Default constructor (necessary even when empty)
        Agent () {}

        //! Advocate the use of ScintArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the ScintArchive plugin
        string get_name () { return "Scint"; }

        //! Return description of this plugin
        string get_description ();

    };

    //! Update the spectra attribute
    void init_spectra ();

    //! Load ScintArchive specific information
    void backend_load (FILE* fptr);

    //! Unload ScintArchive specific information
    void backend_unload (FILE* fptr) const;

    //! Over-ride TimerArchive::correct
    void correct ();

    void set_be_data_size ();

  };


}

#endif


