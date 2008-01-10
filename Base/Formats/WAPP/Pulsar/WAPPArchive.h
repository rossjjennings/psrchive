//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __WAPPArchive_h
#define __WAPPArchive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"

#include "polyco.h"
#include "wapp_header.h"

namespace Pulsar {

  //! Loads and unloads WAPP Pulsar archives
  /*! Class for loading Wideband Arecibo Pulsar Processor (WAPP)
   * data files.  Only handles folded (timing-mode) WAPP data.
   * Does not unload files.
   */
  class WAPPArchive : public BasicArchive {

  public:
    
    //! Default constructor
    WAPPArchive ();

    //! Copy constructor
    WAPPArchive (const WAPPArchive& archive);

    //! Destructor
    ~WAPPArchive ();

    //! Assignment operator
    const WAPPArchive& operator = (const WAPPArchive& archive);
    
    //! Base copy constructor
    WAPPArchive (const Archive& archive);

    //! Base extraction constructor
    WAPPArchive (const Archive&, const std::vector<unsigned>& subint);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive, const std::vector<unsigned>& subints);

    //! Return a new copy-constructed ExampleArchive instance
    WAPPArchive* clone () const;

    //! Return a new extraction-constructed ExampleArchive instance
    WAPPArchive* extract (const std::vector<unsigned>& subints) const;
    
  protected:

    //! Load the Example header information from filename
    virtual void load_header (const char* filename);

    //! Load any polycos from the header
    void load_polycos();

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! Unload the ExampleArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    // Advocates the use of the ExampleArchive plugin
    class Agent;

    //! Enable Advocate template class to access protected Agent class
    friend class Archive::Advocate<WAPPArchive>;

  private:

    //! Initialize all values to null
    void init ();

    //! Polycos from the file
    polyco hdr_polyco;

    //! Header version, size
    uint32_t wapp_hdr_version;
    uint32_t wapp_hdr_size;
    size_t wapp_ascii_hdr_size; // Includes terminating null byte
    size_t wapp_file_size;

    //! The raw WAPP header
    char *rawhdr;

    //! Points to header 
    struct WAPP_HEADER *hdr;

    //! Raw data is ACFs (so needs to be FFTd);
    int raw_data_is_lags;

  };
 

  // Advocates the use of the ExampleArchive plugin
  class WAPPArchive::Agent : public Archive::Advocate<WAPPArchive> {
    
  public:
    
    Agent () { } 
    
    //! Advocate the use of ExampleArchive to interpret filename
    bool advocate (const char* filename);
    
    //! Return the name of the ExampleArchive plugin
    std::string get_name () { return "WAPP"; }
    
    //! Return description of this plugin
    std::string get_description ();
    
  };

}

#endif
