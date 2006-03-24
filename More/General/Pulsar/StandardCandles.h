//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/StandardCandles.h,v $
   $Revision: 1.6 $
   $Date: 2006/03/24 20:43:04 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCalibratorDatabase_h
#define __Pulsar_FluxCalibratorDatabase_h

#include <string>
#include <vector>

#include "Pulsar/FluxCalibrator.h"

namespace Pulsar {

  class FluxCalibrator::Database : public Reference::Able {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Default location of flux calibrator information
    static std::string default_filename;

    //! Default construct from default_filename
    Database ();
    
    //! Construct from the specified filename
    Database (const std::string& filename);
    
    //! Destructor
    ~Database ();
    
    //! Write a text file representing the database
    void unload (const std::string& filename);

    //! Read a text file summary and construct a database
    void load (const std::string& filename);
    
    //! Returns the number of entries in the database
    unsigned size () const { return entries.size(); }
 

    //! Flux Calibration Database Entry
    /*! Entries have two modes of storage:
      
    I.  reference_frequency, reference_flux, spectral_index
    II. log(S) = a_0 + a_1*log(f) + a_2*(log(f))^2 + a_3*(log(f))^4 + ...

    If 'spectral_coeffs' is not empty, mode II is used

    If the config file line starts with '&', then lines as parsed for mode II coefficients

    */
    class Entry {
      
    public:
      
      //! Name of the flux calibrator source
      std::vector<std::string> source_name;

      //! Reference frequency in MHz
      double reference_frequency;

      //! Flux density in mJy
      double reference_flux;

      //! Spectral index
      double spectral_index;

      //! Stores coefficients
      std::vector<double> spectral_coeffs;

      //! Null constructor
      Entry () { init(); }

      //! Construct from an ASCII string
      explicit Entry (std::string& str) { load(str); }

      //! Destructor
      ~Entry();
      
      //! load from ascii string
      void load (const std::string& str);

      //! unload ascii string
      void unload (std::string& str);
      
      //! return true if the source name matches
      bool matches (const std::string& name) const;

      //! Return the flux in mJy at the specified frequency in MHz
      double get_flux_mJy (double MHz);

    protected:
      
      //! Clean slate
      void init ();
      
    };

    //! Returns the best match, given the source name and centre frequency
    Entry match (const std::string& source, double MHz) const;

  protected:
    
    //! vector of entries in the database
    std::vector<Entry> entries;
    
  };


}

#endif
