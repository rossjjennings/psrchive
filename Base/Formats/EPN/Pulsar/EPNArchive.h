//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/Base/Formats/EPN/Pulsar/EPNArchive.h,v $
   $Revision: 1.1 $
   $Date: 2004/07/06 09:41:25 $
   $Author: straten $ */

#ifndef __EPNArchive_h
#define __EPNArchive_h

#include "Pulsar/BasicArchive.h"

namespace Pulsar {

  //! Loads and unloads EPN Pulsar archives
  /*! This EPNArchive class provides an example of how to inherit the
    BasicArchive class and add functionality specific to a file format. 
    By copying the files EPNArchive.h and EPNArchive.C and performing
    simple text-substitution, the skeleton of a new file-format plugin may
    be easily developed. */
  class EPNArchive : public BasicArchive {

  public:
    
    //! Default constructor
    EPNArchive ();

    //! Copy another constructor
    EPNArchive (const Archive& archive);

    //! Copy constructor
    EPNArchive (const EPNArchive& archive);

    //! Extract another constructor
    EPNArchive (const Archive& a, const vector<unsigned>& subints);

    //! Extraction constructor
    EPNArchive (const EPNArchive& a, const vector<unsigned>& subints);

    //! Operator =
    EPNArchive& operator= (const EPNArchive& archive);

    //! Destructor
    ~EPNArchive ();

    //! Copy the contents of an Archive into self
    void copy (const Archive& archive,
	       const vector<unsigned>& subints = none_selected);
    
    //! Return a new copy-constructed EPNArchive instance
    Archive* clone () const;

    //! Return a new extraction-constructed EPNArchive instance
    Archive* extract (const vector<unsigned>& subints) const;
    
  protected:

    //! Load the EPN header information from filename
    virtual void load_header (const char* filename);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! Unload the EPNArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    friend class Archive::Advocate<EPNArchive>;

    //! This class registers the EPNArchive plugin class for use
    class Agent : public Archive::Advocate<EPNArchive> {

      public:

        Agent () { } 

        //! Advocate the use of TimerArchive to interpret filename
        bool advocate (const char* filename);

        //! Return the name of the TimerArchive plugin
        string get_name () { return "EPNArchive"; }
    
        //! Return description of this plugin
        string get_description ();

    };

  private:

    //! Initialize all values to null
    void init ();

  };
 

}

#endif
