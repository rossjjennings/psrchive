/***************************************************************************
 *
 *   Copyright (C) 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/FITSArchive.h"
#include "Pulsar/Pointing.h"
#include "psrfitsio.h"

using namespace std;

void Pulsar::FITSArchive::load_Pointing_columns (fitsfile* fptr)
{
  extra_pointing_columns.clear();

  unsigned ninfo = 0;

  psrfits_read_key (fptr, "NINFO", &ninfo, ninfo, verbose > 2);

  if (ninfo == 0)
  {
    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing_columns no additional Pointing::Info"
           << endl;
    return;
  }

  extra_pointing_columns.resize (ninfo);

  int colnum = get_last_pointing_column (fptr);
  if (verbose > 2)
    cerr << "FITSArchive::load_Pointing_columns last Pointing "
         " colnum=" << colnum << endl;

  for (unsigned i=0; i < ninfo; i++)
  {
    colnum ++;

    string name;
    string desc;
    psrfits_read_key (fptr, "TTYPE", colnum, name, desc);

    string unit;
    string ignore;
    psrfits_read_key (fptr, "TUNIT", colnum, unit, ignore);

    extra_pointing_columns[i].name = name;
    extra_pointing_columns[i].unit = unit;
    extra_pointing_columns[i].description = desc;
    extra_pointing_columns[i].colnum = colnum;

    if (verbose > 2)
      cerr << "FITSArchive::load_Pointing_columns colnum=" << colnum
           << " name='" << name << "'" << endl;
  }
}

