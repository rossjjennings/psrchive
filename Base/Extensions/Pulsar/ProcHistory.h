//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2003 by Aidan Hotan
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

// Class for holding processing history

#ifndef __ProcHistory_h
#define __ProcHistory_h

#include "Pulsar/Archive.h"

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
      std::string date_pro;
      std::string proc_cmd;
      std::string pol_type;
      int npol;
      int nbin;
      int nbin_prd;
      double tbin;
      double ctr_freq;
      int nchan;
      double chanbw;
      int par_corr;
      int fa_corr;
      int rm_corr;
      int dedisp;
      std::string sc_mthd;
      std::string cal_mthd;
      std::string cal_file;
      std::string rfi_mthd;
      std::string ifr_mthd;
      Signal::Scale scale;

      //Destructor
      ~row ();
      
      //Initialisation
      void init ();
      
    };
    
    // The storage array
    std::vector<row> rows;
    
    std::string command_str;
    std::string the_cal_mthd;
    std::string the_sc_mthd;
    std::string the_cal_file;
    std::string the_rfi_mthd;
    std::string the_ifr_mthd;

    ProcHistory::row& get_last ();

    void   set_command_str (std::string str);
    std::string get_command_str ();
    void   set_cal_mthd (std::string str);
    std::string get_cal_mthd ();
    void   set_sc_mthd (std::string str);
    std::string get_sc_mthd ();
    void   set_cal_file (std::string str);
    std::string get_cal_file ();
    void   set_rfi_mthd (std::string str);
    std::string get_rfi_mthd ();
    void   set_ifr_mthd (std::string str);
    std::string get_ifr_mthd ();

    void   add_blank_row ();
    
  private:
    
    void init ();
    
  };
  
}

#endif
