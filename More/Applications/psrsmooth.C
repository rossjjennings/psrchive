/***************************************************************************
 *
 *   Copyright (C) 2008 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Transformation.h"
#include "Pulsar/AdaptiveSmooth.h"
#include "Pulsar/WaveletSmooth.h"

using namespace Pulsar;

//
//! Profile adaptive smoothing application
//
class psrsmooth : public Pulsar::Application
{
public:

  //! Default constructor
  psrsmooth ();

  //! Return extra cmd line args
  std::string get_options() { return "We:"; }

  //! Parse extra command line opts
  bool parse(char code, const std::string& arg);

  //! Extra usage info
  std::string get_usage();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! smoothing method to use
  std::string method;

  //! Extension to append
  std::string ext;

};


psrsmooth::psrsmooth ()
  : Application ("psrsmooth", "Denoise profiles using adaptive smoothing")
{
  method = "sinc";
  ext = "sm";
}

std::string psrsmooth::get_usage() {
  return 
    " -W               Use Wavelet smoothing (default Sinc)\n"
    " -e ext           Append extention to output (default .sm)\n";
}

bool psrsmooth::parse(char code, const std::string& arg) {
  if (code=='W') {
    method = "wavelet";
    return true;
  } else if (code=='e') {
    ext = arg;
    return true;
  } else 
    return false;
}


void psrsmooth::process (Pulsar::Archive* archive)
{

  // We want stokes for 4-pol data, otherwise
  // whatever is input is probably ok.
  if (archive->get_npol()==4)
    archive->convert_state(Signal::Stokes);

  // Set up transformation
  Reference::To< Transformation<Profile> > smooth;
  Reference::To<AdaptiveSmooth> asmooth = NULL;

  if (method=="sinc")
    smooth = new AdaptiveSmooth;
  else if (method=="wavelet")
    smooth = new WaveletSmooth;
  else 
    throw Error(InvalidState, "psrsmooth::process",
        "Unrecognized smoothing method (%s) selected", method.c_str());

  asmooth = dynamic_cast<AdaptiveSmooth*>(smooth.get());

  Reference::To<Integration> subint;

  for (unsigned isub=0; isub<archive->get_nsubint(); isub++) {
    subint = archive->get_Integration(isub);
    for (unsigned ichan=0; ichan<subint->get_nchan(); ichan++) {

      // Smooth pol 0
      (*smooth)(subint->get_Profile(0,ichan));

      // if we're using Fourier-domain, apply same filter to all 
      // other pols
      if (asmooth) asmooth->set_hold(true);

      // Loop over other pols
      for (unsigned ipol=1; ipol<subint->get_npol(); ipol++) 
        (*smooth)(subint->get_Profile(ipol,ichan));

      // Reset hold if needed
      if (asmooth) asmooth->set_hold(false);
    }
  }

  // Unload archive with .sm extension
  archive->unload(archive->get_filename() + "." + ext);

}

static psrsmooth program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

