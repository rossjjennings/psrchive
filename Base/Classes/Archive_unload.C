/***************************************************************************
 *
 *   Copyright (C) 2003 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Predictor.h"

#include "Pulsar/Config.h"

#include "TemporaryFile.h"
#include "dirutil.h"
#include "strutil.h"

#include "Error.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

using namespace std;

mode_t getumask ()
{
  mode_t mask = umask(0);
  umask(mask);
  return mask;
}

/*! To protect data, especially when writing the output archive to a
  file of the same name as the input archive, this method unloads data
  to a temporary file before renaming the archive to the requested
  output file name.  The temporary file will be written using a unique
  filename in the same path as the requested output file. */
void Pulsar::Archive::unload (const char* filename) const
{
  // cerr << "Archive::unload this=" << (void*) this << endl;

  // run all verifications
  try
  {
    verify ();
  }
  catch (Error& error)
  {
    throw error += "Pulsar::Archive::unload";
  }

  string unload_to_filename = unload_filename;
  if (no_clobber && file_exists(filename))
  {
    throw Error (InvalidState, "Pulsar::Archive:unload", "%s exists and no_clobber is true", filename);
  }

  if (filename)
    unload_to_filename = expand(filename);

  if (verbose == 3)
    cerr << "Pulsar::Archive::unload (" << unload_to_filename << ")" << endl;

  std::string path = dirname(unload_to_filename);
  if (path != "." && path != "")
  {
    int path_exists = file_is_directory(path.c_str());
    if (!path_exists)
      cerr << "Pulsar::Archive:unload creating directory " << path << endl;
    int result = makedir(path.c_str());
    if (result != 0)
    {
      throw Error (FailedSys, "Pulsar::Archive:unload", "makedir(" + path + ")");
    }
  }

  TemporaryFile temp (unload_to_filename);

  if (model && unload_cull_predictor)
  {
    create_unload_model();
  }

  try
  {
    if (!can_unload())
    {
      Reference::To<Archive> output = Archive::new_Archive (unload_class);
      output-> copy (*this);
      output-> unload_file (temp.get_filename().c_str());
    }
    else
      unload_file (temp.get_filename().c_str());
  }
  catch (Error& error)
  {
    unload_model = nullptr;
    throw error += "Pulsar::Archive::unload";
  }

  unload_model = nullptr;

  // rename the temporary file with the requested filename
  int ret = rename (temp.get_filename().c_str(), unload_to_filename.c_str());

  if (ret < 0)
    throw Error (FailedSys, "Pulsar::Archive::unload", "failed rename");

  temp.set_removed (true);

  ret = chmod (unload_to_filename.c_str(), 0666 & ~getumask());

  if (ret < 0 && verbose)
  {
    char temp[8];
    sprintf (temp, "%x", 0666 & ~getumask());
    cerr << "Pulsar::Archive::unload WARNING failed chmod ("
         << unload_to_filename << ", " << temp << ")" << endl;
  }

  const_cast<Archive*>(this)->unload_filename = unload_to_filename;
  const_cast<Archive*>(this)->__load_filename = unload_to_filename;
}

void Pulsar::Archive::create_unload_model() const
{
  unload_model = model->clone();

  if (verbose > 2)
  {
    cerr << "Pulsar::Archive::create_unload_model paring down predictor=" << endl;
    model->unload (stderr);
    cerr << "Pulsar::Archive::create_unload_model paring down clone=" << endl;
    unload_model->unload (stderr);
  }

  vector<MJD> epochs( get_nsubint() );
  for (unsigned isub=0; isub < get_nsubint(); isub++)
    epochs[isub] = get_Integration(isub)->get_epoch();

  unload_model->keep (epochs);

  if (verbose > 2)
  {
    cerr << "Pulsar::Archive::create_unload_model pared down result=" << endl;
    unload_model->unload (stderr);
  }
}

Pulsar::Option<bool> Pulsar::Archive::unload_cull_predictor
(
 "Archive::unload_cull_predictor",
 true,
 "Default policy for unloading phase predictor coefficients",
 "When unloading a file, cull the predictor down to only those \n"
 "sets of coefficients required to describe sub-integrations."
);

Pulsar::Option<bool> Pulsar::Archive::no_clobber
(
 "Archive::no_clobber",
 false,
 "Default policy for overwriting archive files",
 "This variables controls the behaviour of psrchive during attempt \n"
 "to overwrite an existing file while unloading a new archive."
);
