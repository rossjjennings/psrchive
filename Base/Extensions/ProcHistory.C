//-*-C++-*-

#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "Pulsar/ProcHistory.h"

// //////////////////////////////////////////////////
// ProcHistory methods
//

Pulsar::ProcHistory::ProcHistory (const ProcHistory& extension)
{
  rows = extension.rows;
}

const Pulsar::ProcHistory& 
Pulsar::ProcHistory::operator= (const ProcHistory& extension)
{
  rows = extension.rows;
  return *this;
}

Pulsar::ProcHistory::~ProcHistory ()
{
}

void Pulsar::ProcHistory::init ()
{
  rows.resize (0);
}

Pulsar::ProcHistory::row* Pulsar::ProcHistory::get_last ()
{
  return rows[rows.size()-1];
}

void Pulsar::ProcHistory::add_blank_row ()
{
  rows.push_back(new row ());
}

// //////////////////////////////////////////////////
// ProcHistory::row methods
//

Pulsar::ProcHistory::row::~row ()
{
  delete [] date_pro;
  delete [] proc_cmd;
  delete [] pol_type;
  delete [] sc_mthd;
  delete [] cal_mthd;
  delete [] cal_file;
  delete [] rfi_mthd;
}

void Pulsar::ProcHistory::row::init ()
{
  date_pro = new char[24];
  proc_cmd = new char[80];
  pol_type = new char[8];
  sc_mthd  = new char[32];
  cal_mthd = new char[32];
  cal_file = new char[32];
  rfi_mthd = new char[32];
  
  sprintf(date_pro, "%s", "unset");
  sprintf(proc_cmd, "%s", "unset");
  sprintf(pol_type, "%s", "unset");
  sprintf(sc_mthd,  "%s", "NONE");
  sprintf(cal_mthd, "%s", "NONE");
  sprintf(cal_file, "%s", "NONE");
  sprintf(rfi_mthd, "%s", "NONE");
  
  npol     = 0;
  nbin     = 0;
  nbin_prd = 0;
  tbin     = 0.0;
  ctr_freq = 0.0;
  nchan    = 0;
  chanbw   = 0.0;
  par_corr = 0;
  rm_corr  = 0;
  dedisp   = 0;
}
