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

Pulsar::ProcHistory::row& Pulsar::ProcHistory::get_last ()
{
  return rows.back();
}

void Pulsar::ProcHistory::add_blank_row ()
{
  rows.push_back(row());
}

// //////////////////////////////////////////////////
// ProcHistory::row methods
//

Pulsar::ProcHistory::row::~row ()
{
}

void Pulsar::ProcHistory::row::init ()
{
  date_pro = "unset";
  proc_cmd = "unset";
  pol_type = "unset";
  sc_mthd  = "NONE";
  cal_mthd = "NONE";
  cal_file = "NONE";
  rfi_mthd = "NONE";
  
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
