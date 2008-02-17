//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifndef __PRESTOArchive_h
#define __PRESTOArchive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"

#include <prepfold.h>

namespace Pulsar {

  //! Loads and unloads PRESTO Prepfold (.pfd) pulsar archives
  class PRESTOArchive : public BasicArchive {

  public:
    
    //! Default constructor
    PRESTOArchive ();

    //! Copy constructor
    PRESTOArchive (const PRESTOArchive& archive);

    //! Destructor
    ~PRESTOArchive ();

    //! Assignment operator
    const PRESTOArchive& operator = (const PRESTOArchive& archive);
    
    //! Base copy constructor
    PRESTOArchive (const Archive& archive);

    //! Base extraction constructor
    PRESTOArchive (const Archive&, const std::vector<unsigned>& subint);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive, const std::vector<unsigned>& subints);

    //! Return a new copy-constructed PRESTOArchive instance
    PRESTOArchive* clone () const;

    //! Return a new extraction-constructed PRESTOArchive instance
    PRESTOArchive* extract (const std::vector<unsigned>& subints) const;
    
  protected:

    //! Load the PRESTO header information from filename
    virtual void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! Unload the PRESTOArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    // Advocates the use of the PRESTOArchive plugin
    class Agent;

    //! Enable Advocate template class to access protected Agent class
    friend class Archive::Advocate<PRESTOArchive>;

  private:

    //! The raw prepfold structure
    prepfoldinfo pfd;

    //! Raw header size
    int header_size;

    //! Initialize all values to null
    void init ();

    //! Read, allocate a presto-style string from current file position
    int read_string(char **out, FILE *f);

    //! Read, a presto position struct from current file position
    void read_position(position *out, FILE *f);

    //! Test that header params fall in acceptable ranges
    int test_param_range(std::string &whynot);

    //! Does file endian need to be changed?
    int endian_swap;

    //! Change endianness of header struct
    void change_header_endian();

    //! Change endianness of foldstats struct
    void change_foldstats_endian(foldstats *f);

  };
 

  // Advocates the use of the ExampleArchive plugin
  class PRESTOArchive::Agent : public Archive::Advocate<PRESTOArchive> {
    
  public:
    
    Agent () { } 
    
    //! Advocate the use of PRESTOArchive to interpret filename
    bool advocate (const char* filename);
    
    //! Return the name of the PRESTOArchive plugin
    std::string get_name () { return "PRESTOArchive"; }
    
    //! Return description of this plugin
    std::string get_description ();
    
  };

}

#endif
