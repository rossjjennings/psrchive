//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2016 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Formats/UVM/Pulsar/UVMArchive.h

#ifndef __UVMArchive_h
#define __UVMArchive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"
#include "Pulsar/Config.h"

namespace Pulsar {

  //! Loads UVM data files
  class UVMArchive : public BasicArchive {

  public:

    //! Load the phase bins identified as part of the off-pulse baseline
    static Option<bool> include_offpulse_region;
    
    //! Default constructor
    UVMArchive ();

    //! Copy constructor
    UVMArchive (const UVMArchive& archive);

    //! Destructor
    ~UVMArchive ();

    //! Assignment operator
    const UVMArchive& operator = (const UVMArchive& archive);
    
    //! Base copy constructor
    UVMArchive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    //! Return a new copy-constructed UVMArchive instance
    UVMArchive* clone () const;

    //! Load the UVM header information from filename
    virtual void load_header (const char* filename);
    
  protected:

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! The unload_file method is implemented
    bool can_unload () const { return false; }

    //! Unload the UVMArchive (header and Integration data) to filename
    void unload_file (const char* filename) const { }

    friend class Archive::Advocate<UVMArchive>;

    //! This class registers the UVMArchive plugin class for use
    class Agent : public Archive::Advocate<UVMArchive> {

      public:

        Agent () { } 

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        std::string get_name () { return "UVM"; }
    
        //! Return description of this plugin
        std::string get_description ();

    };

    //! The integration length
    double integration_length;

    //! The epoch of the observation
    MJD epoch;

    //! The folding period
    double period;

  private:

    //! Initialize all values to null
    void init ();

    void* header_ptr;
    int program;

  };
 

}

#endif
