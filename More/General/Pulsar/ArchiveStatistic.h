//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2020 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// psrchive/More/General/Pulsar/ArchiveStatistic.h

#ifndef __ArchiveStatistic_h
#define __ArchiveStatistic_h

#include "Pulsar/HasArchive.h"
#include "Identifiable.h"
#include "TextInterface.h"

namespace Pulsar {

  //! Commmon statistics that can be derived from an Archive
  /*! Each statistic can be computed as a function of sub-integration,
      frequency-channel, and polarization */

  class ArchiveStatistic : public Identifiable, public HasArchive
  {
  protected:

    //! Construct without a name and description
    /*! Available only to derived classes */
    ArchiveStatistic ();

    //! File to which auxiliary data will be printed
    FILE* fptr;
    
  public:

    //! Create a new instance of ArchiveStatistic based on name
    static ArchiveStatistic* factory (const std::string& name);

    //! Construct with a name and description
    ArchiveStatistic (const std::string& name,
                      const std::string& description);

    //! Derived types define the value returned
    virtual double get () = 0;

    //! Return a text interface that can be used to configure this instance
    virtual TextInterface::Parser* get_interface () = 0;

    //! Derived types must also define clone method
    virtual ArchiveStatistic* clone () const = 0;

    //! Set the file to which auxiliary data will be printed
    virtual void set_file (FILE* f) { fptr = f; }

    //! Close the file to which auxiliary data were printed
    virtual void fclose ();
  };

  std::ostream& operator<< (std::ostream&, ArchiveStatistic*);

  std::istream& operator>> (std::istream&, ArchiveStatistic*&);

}

#endif

