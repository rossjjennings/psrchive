//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/ExampleArchive.h,v $
   $Revision: 1.3 $
   $Date: 2003/08/14 19:09:14 $
   $Author: straten $ */

#ifndef __ExampleArchive_h
#define __ExampleArchive_h

#include "Pulsar/BasicArchive.h"

namespace Pulsar {

  //! Loads and unloads Example Pulsar archives
  /*! This ExampleArchive class provides an example of how to inherit the
    BasicArchive class and add functionality specific to a file format. 
    By copying the files ExampleArchive.h and ExampleArchive.C and performing
    simple text-substitution, the skeleton of a new file-format plugin may
    be easily developed. */
  class ExampleArchive : public BasicArchive {

  public:
    
    //! Default constructor
    ExampleArchive ();

    //! Copy another constructor
    ExampleArchive (const Archive& archive);

    //! Copy constructor
    ExampleArchive (const ExampleArchive& archive);

    //! Extract another constructor
    ExampleArchive (const Archive& a, const vector<unsigned>& subints);

    //! Extraction constructor
    ExampleArchive (const ExampleArchive& a, const vector<unsigned>& subints);

    //! Operator =
    ExampleArchive& operator= (const ExampleArchive& archive);

    //! Destructor
    ~ExampleArchive ();

    //! Copy the contents of an Archive into self
    void copy (const Archive& archive,
	       const vector<unsigned>& subints = none_selected);
    
    //! Return a new copy-constructed ExampleArchive instance
    Archive* clone () const;

    //! Return a new extraction-constructed ExampleArchive instance
    Archive* extract (const vector<unsigned>& subints) const;
    
  protected:

    //! Load the Example header information from filename
    virtual void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! Unload the ExampleArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    friend class Archive::Advocate<ExampleArchive>;

    //! This class registers the ExampleArchive plugin class for use
    class Agent : public Archive::Advocate<ExampleArchive> {

      public:

        Agent () { } 

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        string get_name () { return "ExampleArchive"; }
    
        //! Return description of this plugin
        string get_description ();

    };

  private:

    //! Initialize all values to null
    void init ();

  };
 

}

#endif
