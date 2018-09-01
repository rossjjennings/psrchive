//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Base/Classes/Pulsar/EPOSArchive.h,v $
   $Revision: 1.13 $
   $Date: 2008/11/07 22:15:36 $
   $Author: straten $ */

#ifndef __EPOSArchive_h
#define __EPOSArchive_h

#include "Pulsar/BasicArchive.h"
#include "Pulsar/Agent.h"
#include "eposio.h"

namespace Pulsar {

  //! Loads and unloads EPOS Pulsar archives
  /*! This EPOSArchive class provides an example of how to inherit the
    BasicArchive class and add functionality specific to a file format. 
    By copying the files EPOSArchive.h and EPOSArchive.C and performing
    simple text-substitution, the skeleton of a new file-format plugin may
    be easily developed. */
  class EPOSArchive : public BasicArchive {

  public:
    
    //! Default constructor
    EPOSArchive ();

    //! Copy constructor
    EPOSArchive (const EPOSArchive& archive);

    //! Destructor
    ~EPOSArchive ();

    //! Assignment operator
    const EPOSArchive& operator = (const EPOSArchive& archive);
    
    //! Base copy constructor
    EPOSArchive (const Archive& archive);

    //! Copy all of the class attributes and the selected Integration data
    void copy (const Archive& archive);

    //! Return a new copy-constructed EPOSArchive instance
    EPOSArchive* clone () const;


  protected:

    //! Load the EPOS header information from filename
    virtual void load_header (const char* filename);

	virtual Integration* new_Integration (const Integration* copy_this = 0);

    //! Load the specified Integration from filename, returning new instance
    virtual Integration*
    load_Integration (const char* filename, unsigned subint);

    //! The unload_file method is not implemented
    bool can_unload () const { return false; }

    //! Unload the EPOSArchive (header and Integration data) to filename
    virtual void unload_file (const char* filename) const;

    // Advocates the use of the EPOSArchive plugin
    class Agent;

    //! Enable Advocate template class to access protected Agent class
    friend class Archive::Advocate<EPOSArchive>;

    // local variables specific to EPOS archive
    
    unsigned int current_record;
    
    unsigned long ref_syncusec;

    eposhdr_t eposhdr;

    eposdata_t eposdata;


  private:

    //! Initialize all values to null
    void init ();

	//! Get the source name                                                                                             
	std::string get_source (eposhdr_t *eposhdr);

	MJD get_startdate(const char *filename, eposhdr_t *eposhdr, unsigned subint);

	double get_intlen (eposhdr_t *eposhdr);

	double get_foldperiod (eposhdr_t *eposhdr);

  };
 

  // Advocates the use of the EPOSArchive plugin
  class EPOSArchive::Agent : public Archive::Advocate<EPOSArchive> {
    
  public:
    
    Agent () { } 
    
    //! Advocate the use of EPOSArchive to interpret filename
    bool advocate (const char* filename);
    
    //! Return the name of the EPOSArchive plugin
    std::string get_name () { return "EPOS"; }
    
    //! Return description of this plugin
    std::string get_description () ;
    
  };

}

#endif
