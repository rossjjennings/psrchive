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

#include "Pulsar/ArchiveExtension.h"
#include "TextInterface.h"

namespace Pulsar {
  
  class ProcHistory : public Archive::Extension {
    
  public:
    
    //Default constructor
    ProcHistory ();
    
    //! Clone method
    ProcHistory* clone () const { return new ProcHistory( *this ); }
    
    //! Get the text interface
    TextInterface::Parser* get_interface();
    
    // Text interface to a ProcHistory instance
    class Interface : public TextInterface::To<ProcHistory>
    {
      public:
	Interface( ProcHistory *s_instance = NULL );
    };

    //! Return an abbreviated name that can be typed relatively quickly
    std::string get_short_name () const { return "hist"; }

    ///////////////////////////////////////////////////////////////
    // The information container
    
    class row {
      
    public:
      
      // Default constructor
      row ();
      
      // Text interface to a ProcHistory instance
      class Interface : public TextInterface::To<row>
      {
      public:
	Interface( row *s_instance = NULL );
      };

      int get_nbin() const { return nbin; }
      int get_npol() const { return npol; }
      int get_nsub() const { return nsub; }
      int get_nchan() const { return nchan; }
      int get_nbin_prd() const { return nbin_prd; }
      
      // Row label
      int index;
      
      // Information in the row
      std::string date_pro;
      std::string proc_cmd;
      std::string pol_type;
      int nsub;
      int npol;
      int nbin;
      int nbin_prd;
      double tbin;
      double ctr_freq;
      int nchan;
      double chan_bw;
      double ref_freq;
      double dispersion_measure;
      double rotation_measure;
      int pr_corr;
      int fd_corr;
      int be_corr;
      int rm_corr;
      int dedisp;

      int aux_rm_corr;
      int aux_dm_corr;

      std::string dds_mthd;
      std::string sc_mthd;
      std::string cal_mthd;
      std::string cal_file;
      std::string rfi_mthd;

      std::string aux_rm_model;
      std::string aux_dm_model;

      std::string scale;      
    };
    
    // The storage array
    std::vector<row> rows;

    //! Get the number of rows
    unsigned get_nrow () const;

    //! Get the ith row
    row* get_row (unsigned i);
    const row* get_row (unsigned i) const;

    std::string command_str;
    std::string the_cal_mthd;
    std::string the_sc_mthd;
    std::string the_cal_file;
    std::string the_rfi_mthd;

    ProcHistory::row& get_first ();
    ProcHistory::row& get_last ();

    void   set_command_str (const std::string&);
    std::string get_command_str ();
    void   set_cal_mthd (const std::string&);
    std::string get_cal_mthd ();
    void   set_sc_mthd (const std::string&);
    std::string get_sc_mthd ();
    void   set_cal_file (const std::string&);
    std::string get_cal_file () const { return the_cal_file; }
    void   set_rfi_mthd (const std::string&);
    std::string get_rfi_mthd ();

    int get_last_nbin_prd () const { return rows.back().nbin_prd; }
    double get_last_tbin () const { return rows.back().tbin; }
    double get_last_chan_bw () const { return rows.back().chan_bw; }

    void   add_blank_row ();
    
  private:
    
    void check_irow (unsigned irow) const;
  };
  
}

#endif
