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

int Pulsar::FITSArchive::get_last_pointing_column (fitsfile* fptr) const
{
  const char* name = "TEL_ZEN";
  int colnum = 0;
  int status = 0;

  fits_get_colnum (fptr, CASEINSEN, const_cast<char*>(name),
                   &colnum, &status);

  if (status)
    throw FITSError (status, "FITSArchive::get_last_pointing_column",
                     "fits_get_colnum " + string(name));

  return colnum;
}

void Pulsar::FITSArchive::add_Pointing_columns (fitsfile* fptr) const
{
  extra_pointing_columns.clear();

  if (verbose > 2)
    cerr << "Pulsar::FITSArchive::add_Pointing_columns "
         "nsubint=" << get_nsubint() << endl;
 
  for (unsigned isub=0; isub < get_nsubint(); isub++)
  {
    const Pointing* pointing = get_Integration(isub) -> get<Pointing>();
    if (!pointing)
      continue;

    unsigned ninfo = pointing->get_ninfo();

    if (ninfo == 0)
      continue;

    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::add_Pointing_columns subint="
           << isub << " has extra Pointing::Info" << endl;

    if (extra_pointing_columns.size() &&
        extra_pointing_columns.size() != ninfo)
    {
      warning << "Pulsar::FITSArchive::add_Pointing_columns "
         "subint=" << isub << " has " << ninfo << " extra Pointing::Info, "
         "whereas " << extra_pointing_columns.size() 
         << " extra columns have been identified" << endl;
    }

    for (unsigned i=0; i < ninfo; i++)
    {
      const Pointing::Info* info = pointing->get_info(i);

      bool found = false;
      for (unsigned icol=0; icol < extra_pointing_columns.size(); icol++)
      {
        pointing_info_column existing = extra_pointing_columns[icol];
        if (existing.name == info->get_name())
        {
          found = true;
          if (existing.unit != info->get_unit())
          {
            warning << "Pulsar::FITSArchive::add_Pointing_columns "
               "subint=" << isub << " has extra Pointing::Info "
               "with name='" << info->get_name() << "' "
               "and unit='" << info->get_unit() << "' "
               "not equal to the unit='" << existing.unit << "' "
               "already set for the column of the same name" << endl;
          }

          if (existing.description != info->get_description())
          {
            warning << "Pulsar::FITSArchive::add_Pointing_columns "
               "subint=" << isub << " has extra Pointing::Info "
               "with name='" << info->get_name() << "' "
               "and description='" << info->get_unit() << "' "
               "not equal to the description='" << existing.description << "' "
               "already set for the column of the same name" << endl;
          }
        }
      }

      if (!found)
      {
        if (verbose > 2)
          cerr << "Pulsar::FITSArchive::add_Pointing_columns "
            "subint=" << isub << " adding extra Pointing::Info " 
            "with name='" << info->get_name() << "'" << endl;
 
        pointing_info_column col;
        col.name = info->get_name();
        col.unit = info->get_unit();
        col.description = info->get_description();

        extra_pointing_columns.push_back(col);
      }
    }
  }

  unsigned ninfo = extra_pointing_columns.size();

  psrfits_update_key (fptr, "NINFO", ninfo);

  if (ninfo == 0)
  {
    if (verbose > 2)
      cerr << "Pulsar::FITSArchive::add_Pointing_columns"
        " no subints have extra Pointing::Info" << endl;

    return;
  }

  int colnum = get_last_pointing_column (fptr);
  if (verbose > 2)
    cerr << "FITSArchive::add_Pointing_columns last Pointing "
         " colnum=" << colnum << endl;

  for (unsigned i=0; i < ninfo; i++)
  {
    colnum ++;

    string name = extra_pointing_columns[i].name;
    string unit = extra_pointing_columns[i].unit;
    string desc = extra_pointing_columns[i].description;

    int status = 0;
    fits_insert_col (fptr, colnum, 
                     const_cast<char*>(name.c_str()), "1D", &status);

    if (status)
      throw FITSError (status, "FITSArchive::unload_Pointing",
                       "fits_insert_col " + name);

    psrfits_update_key (fptr, "TTYPE", colnum, name.c_str(), desc.c_str());
    psrfits_update_key (fptr, "TUNIT", colnum, unit.c_str(), "units of field");

    extra_pointing_columns[i].colnum = colnum;
  }
}
