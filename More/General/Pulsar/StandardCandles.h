//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/General/Pulsar/StandardCandles.h,v $
   $Revision: 1.1 $
   $Date: 2004/10/07 15:58:07 $
   $Author: straten $ */

#ifndef __Pulsar_FluxCalibratorDatabase_h
#define __Pulsar_FluxCalibratorDatabase_h

#include <string>
#include <vector>

namespace Pulsar {

  //! Pulsar Calibrator Observation Database
  class FluxCalibratorDatabase {

  public:

    //! Verbosity flag
    static bool verbose;

    //! Null constructor
    FluxCalibratorDatabase ();
    
    //! Construct a database from a pre-built ascii file
    FluxCalibratorDatabase (const std::string& filename);
    
    //! Destructor
    ~FluxCalibratorDatabase ();
    
    //! Write a text file representing the database
    void unload (const std::string& filename);

    //! Read a text file summary and construct a database
    void load (const std::string& filename);
    
    //! Returns the number of entries in the database
    unsigned size () const { return entries.size(); }
 

    //! Flux Calibration Database Entry
    class Entry {
      
    public:
      
      //! Name of the flux calibrator source
      std::string source_name;

      //! Reference frequency in MHz
      double reference_frequency;

      //! Flux density in mJy
      double reference_flux;

      //! Spectral index
      double spectral_index;

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
      
      //! Return the flux in mJy at the specified frequency in MHz
      double get_flux_mJy (double MHz);

    protected:
      
      //! Clean slate
      void init ();
      
    };

    Entry match (const std::string& source);

  protected:
    
    // list of entries in the database
    std::vector<Entry> entries;
    
  };


}

#endif
