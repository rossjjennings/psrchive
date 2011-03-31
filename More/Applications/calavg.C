/***************************************************************************
 *
 *   Copyright (C) 2011 by Jonathan Khoo
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/Application.h"
#include "Pulsar/Archive.h"
#include "Pulsar/StandardOptions.h"

#include "Error.h"

using std::cerr;
using std::cout;
using std::endl;

//! Calibration file (Pulsar Archive) averaging application.
class calavg : public Pulsar::Application
{
  public:

    calavg();

    void process(Pulsar::Archive*);

  protected:

    //! Add command line options
    void add_options(CommandLine::Menu&);

  private:

    vector<Pulsar::Archive
};

void calavg::process(Pulsar::Archive* archive)
{
  // Store each archive into a vector
  std::cerr << archive->get_filename() << std::endl;
}

void calavg::add_options(CommandLine::Menu& menu)
{
  CommandLine::Argument* arg = 0;
}

static calavg program;

int main(int argc, char *argv[])
{
  return program.main(argc, argv);
}

calavg::calavg() : Pulsar::Application("calavg", "averages cal files")
{
  add( new Pulsar::StandardOptions );
}

