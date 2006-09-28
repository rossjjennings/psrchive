//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// Class for holding digitiser statistics

#ifndef __DigitiserStatistics_h
#define __DigitiserStatistics_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  class DigitiserStatistics : public Pulsar::Archive::Extension {
    
  public:
    
    //Default constructor
    DigitiserStatistics ();
    
    // Copy constructor
    DigitiserStatistics (const DigitiserStatistics& extension);
    
    // Operator =
    const DigitiserStatistics& operator= (const DigitiserStatistics&);
    
    // Destructor
    ~DigitiserStatistics ();
    
    //! Clone method
    DigitiserStatistics* clone () const 
    { return new DigitiserStatistics( *this ); }

    ///////////////////////////////////////////////////////////////
    // The information container
    
    class row {
                                                                                
    public:

      row () { init(); }
      ~row ();

      int index;
      
      std::string dig_mode;
      int ndigr;
      int nlev;
      int ncycsub;
      std::string diglev;
      
      std::vector<float> data;
      
    private:
      
      void init ();
      
    };

    std::vector<row> rows;
    
    DigitiserStatistics::row& get_row (unsigned i);
    DigitiserStatistics::row& get_last ();

    void push_blank_row ();

  private:
    
    void init ();
    
  };
  
}

#endif
