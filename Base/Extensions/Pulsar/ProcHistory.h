//-*-C++-*-

// Class for holding processing history

#ifndef __ProcHistory_h
#define __ProcHistory_h

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"

#include "environ.h"

namespace Pulsar {
  
  class ProcHistory : public Pulsar::Archive::Extension {
    
  public:
    
    //Default constructor
    ProcHistory ();
    
    // Copy constructor
    ProcHistory (const Pulsar::ProcHistory& extension);
    
    // Operator =
    const ProcHistory& operator= (const ProcHistory& extension);
    
    // Destructor
    ~ProcHistory ();
    
    //! Clone method
    ProcHistory* clone () const { return new ProcHistory( *this ); }

    ///////////////////////////////////////////////////////////////
    // The information container
    
    class row {
      
    public:
      
      // Default constructor
      row () { init(); }
      
      // Row label
      int index;
      
      // Information in the row
      string date_pro;
      string proc_cmd;
      string pol_type;
      int npol;
      int nbin;
      int nbin_prd;
      double tbin;
      double ctr_freq;
      int nchan;
      double chanbw;
      int par_corr;
      int rm_corr;
      int dedisp;
      string sc_mthd;
      string cal_mthd;
      string cal_file;
      string rfi_mthd;
      
      //Destructor
      ~row ();
      
      //Initialisation
      void init ();
      
    };
    
    // The storage array
    vector<row> rows;
    
    string command_str;
    string the_cal_mthd;
    string the_sc_mthd;
    string the_cal_file;
    string the_rfi_mthd;

    ProcHistory::row& get_last ();

    void   set_command_str (string str);
    string get_command_str ();
    void   set_cal_mthd (string str);
    string get_cal_mthd ();
    void   set_sc_mthd (string str);
    string get_sc_mthd ();
    void   set_cal_file (string str);
    string get_cal_file ();
    void   set_rfi_mthd (string str);
    string get_rfi_mthd ();
    
    void   add_blank_row ();
    
  private:
    
    void init ();
    
  };
  
}

#endif
