//-*-C++-*-

// Class for holding digitiser statistics

#ifndef __DigitiserStatistics_h
#define __DigitiserStatistics_h

#include "Pulsar/Archive.h"

namespace Pulsar {
  
  class DigitiserStatistics : public Pulsar::Archive::Extension {
    
  public:
    
    //Default constructor
    DigitiserStatistics () { init(); }
    
    // Copy constructor
    DigitiserStatistics (const Pulsar::DigitiserStatistics& extension);
    
    // Operator =
    const DigitiserStatistics& operator= (const DigitiserStatistics& extension);
    
    // Destructor
    ~DigitiserStatistics ();
    
    ///////////////////////////////////////////////////////////////
    // The information container
    
    class row {
                                                                                
    public:

      row () { init(); }
      ~row ();

      int index;
      
      string dig_mode;
      int ndigr;
      int nlev;
      int ncycsub;
      string diglev;
      
      vector<float> data;
      
    private:
      
      void init ();
      
    };

    vector<row> rows;
    
    DigitiserStatistics::row& get_row (unsigned i);
    DigitiserStatistics::row& get_last ();

    void push_blank_row ();

  private:
    
    void init ();
    
  };
  
}

#endif
