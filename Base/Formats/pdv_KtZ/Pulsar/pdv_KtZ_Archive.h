//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2018 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/Base/Formats/pdv_KtZ_/Pulsar/pdv_KtZ_Archive.h

#ifndef __pdv_KtZ_Archive_h
#define __pdv_KtZ_Archive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"

namespace Pulsar {

  //! Loads data from the ASCII output by <kbd>pdv -KtZ</kbd>
  class pdv_KtZ_Archive : public BasicArchive {

  public:
    
    //! Default constructor
    pdv_KtZ_Archive ();

    //! Copy constructor
    pdv_KtZ_Archive (const pdv_KtZ_Archive& archive);

    //! Destructor
    ~pdv_KtZ_Archive ();

    //! Assignment operator
    const pdv_KtZ_Archive& operator = (const pdv_KtZ_Archive& archive);
    
    //! Base copy constructor
    pdv_KtZ_Archive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    //! Return a new copy-constructed pdv_KtZ_Archive instance
    pdv_KtZ_Archive* clone () const;

    //! Load the pdv_KtZ_ header information from filename
    virtual void load_header (const char* filename);
    
  protected:

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! The unload_file method is implemented
    bool can_unload () const { return false; }

    //! Unload the pdv_KtZ_Archive (header and Integration data) to filename
    void unload_file (const char* filename) const;

    friend class Archive::Advocate<pdv_KtZ_Archive>;

    //! This class registers the pdv_KtZ_Archive plugin class for use
    class Agent : public Archive::Advocate<pdv_KtZ_Archive> {

      public:

        Agent () { } 

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        std::string get_name () { return "pdv_KtZ"; }
    
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

  };
 

}

#endif
