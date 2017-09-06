/***************************************************************************
 *
 *   Copyright (C) 2010 by Paul Demorest
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

using namespace std;

#include "Pulsar/Application.h"
#include "Pulsar/StandardOptions.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Integration.h"
#include "Pulsar/Profile.h"
#include "Pulsar/DynamicSpectrum.h"
#include "Pulsar/StandardFlux.h"

using namespace Pulsar;

//
//! Produce dynamic spectra from Archives
//
class psrflux : public Pulsar::Application
{
public:

  //! Default constructor
  psrflux ();

  //! One-time setup
  void setup ();

  //! Process the given archive
  void process (Pulsar::Archive*);

  //! Extension to append
  std::string ext;

  //! Flux calculation
  DynamicSpectrum ds;

  //! For profile aligment if needed
  ProfileShiftFit psf;

  //! Standard profile file
  std::string stdfile;

  //! Standard archive
  Reference::To<Archive> stdarch;

  //! Setup to use the given file as a standard
  void set_standard(Archive *arch);

  //! Align w/ each input profile separately
  bool align;

  //! Disable fits for alignment
  bool noalign;

protected:

  //! Add command line options
  void add_options (CommandLine::Menu&);

  //! Standard preprocessing options
  Pulsar::StandardOptions standard_options;

};


psrflux::psrflux ()
  : Application ("psrflux", "Produce dynamic spectra from Archives")
{
  ext = "ds";
  stdfile = "";
  align = false;
  noalign = false;

  add( &standard_options );
}

void psrflux::add_options (CommandLine::Menu& menu)
{
  CommandLine::Argument* arg;

  menu.add("\n" "Dynamic spectrum options:");

  arg = menu.add (ext, 'e', "ext");
  arg->set_help ("Append extention to output (default .ds)");

  arg = menu.add (stdfile, 's', "std");
  arg->set_help ("Standard profile file");

  arg = menu.add (align, 'a', "align");
  arg->set_help ("Align standard with each profile separately");

  arg = menu.add (noalign, 'A', "noalign");
  arg->set_help ("No fit for profile alignment at all");

}

void psrflux::set_standard(Archive *arch)
{
  // Convert
  stdarch = arch->total();
  stdarch->convert_state(Signal::Intensity);

  // Set up DS calculation
  Reference::To<StandardFlux> flux = new StandardFlux;
  flux->set_fit_shift(true); // always init with true, choose later
  flux->set_standard(stdarch->get_Profile(0,0,0));
  ds.set_flux_method(flux);
}

void psrflux::setup ()
{

  // If a standard was given, load it.  Otherwise compute a standard
  // from each input file (with warning).
  if (stdfile != "") {
    Reference::To<Archive> arch = Archive::load(stdfile);
    set_standard(arch);
  } else {
    cerr 
      << "psrflux: No standard given, will \"self-standard\" each file."
      << endl;
  }

}

void psrflux::process (Pulsar::Archive* archive)
{
  // Convert to total intensity
  archive->convert_state(Signal::Intensity);

  // Set self-standard if needed
  if (stdfile=="") set_standard(archive);

  // Test for single-profile data
  bool single_profile = archive->get_nsubint()==1 && archive->get_nchan()==1;

  // Access to the flux computation
  StandardFlux *flux = dynamic_cast<StandardFlux*>(ds.get_flux_method().get());

  // If shifts not fit, need to dedisperse and possibly align total
  // with standard.
  if (noalign) {
    archive->dedisperse();
    flux->set_fit_shift(false);
  } else if (align==false && single_profile==false) {
    archive->dedisperse();
    Reference::To<Archive> arch_tot = archive->total();
    Estimate<double> shift = 
      arch_tot->get_Profile(0,0,0)->shift(stdarch->get_Profile(0,0,0));
    stdarch->get_Profile(0,0,0)->rotate_phase(-1.0*shift.get_value());
    flux->set_fit_shift(false);
  } else {
    flux->set_fit_shift(true);
  }

  // Compute DS
  ds.set_Archive(archive);
  ds.compute();

  // Unload archive with .sm extension
  std::string outf = archive->get_filename() + "." + ext;
  cerr << "psrflux: unloading " << outf << endl;
  ds.unload(outf, command);

}

static psrflux program;

int main (int argc, char** argv)
{
  return program.main (argc, argv);
}

