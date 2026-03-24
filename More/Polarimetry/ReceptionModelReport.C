
/***************************************************************************
 *
 *   Copyright (C) 2012 - 2023 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/ReceptionModelReport.h"
#include "ThreadContext.h"
#include "dirutil.h"

#include <fstream>
using namespace std;

#if HAVE_PTHREAD
static ThreadContext* context = new ThreadContext;
#else
static ThreadContext* context = 0;
#endif

std::string Calibration::ReceptionModel::Report::make_reports_path (const string& subdir)
{
  ThreadContext::Lock lock (context);

  if (path.empty())
    path = "pcm_reports";

  string fullpath = path;
  if (!subdir.empty())
    fullpath += "/" + subdir;

  if (makedir(fullpath.c_str()) < 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::Report::make_reports_path",
                "makedir(" + fullpath + ") failed");

  return fullpath;
}

void Calibration::ReceptionModel::Report::report ()
{
  // cerr << "Calibration::ReceptionModel::Report::report filename=" << filename << endl;

  if (filename.empty())
    throw Error( InvalidState, "Calibration::ReceptionModel::Report::report", "filename not set");

  string fullpath = make_reports_path ();

  string fullname = fullpath + "/" + filename;
  ofstream os ( fullname.c_str() );
  if (!os)
    throw Error( FailedSys, "Calibration::ReceptionModel::Report::report", "ofstream (" + fullname + ")" );
 
  report (os);
}

