
#include "Pulsar/Archive.h"
#include "string_utils.h"
#include "dirutil.h"
#include "Error.h"

#include <iostream>
#include <unistd.h>

void usage() {

  cout << 
    "A simple program to convert between pulsar archive formats \n"
    "Usage: psrconv (options) <filenames> \n"
    "Where the options are as follows: \n"
    "     -o fmt The output format [default PSRFITS] \n"
    "     -h     This help message \n"
    "     -q     Quiet mode \n"
    "     -v     Verbose mode \n"
    "     -V     Very verbose mode \n"
    "\n"
    "Available output formats:" << endl;

  Pulsar::Archive::agent_list ();

}

int main(int argc, char *argv[])
{
  string output_format = "PSRFITS";
  string unload_cal_ext = ".cf";
  string unload_psr_ext = ".rf";

  char* metafile = 0;

  bool verbose = false;
  bool quiet = false;

  int gotc = 0;
  while ((gotc = getopt(argc, argv, "hM:o:qVv")) != -1) {
    switch (gotc) {

    case 'o':
      output_format = optarg;
      break;

    case 'h':
      usage();
      return 0;

    case 'M':
      metafile = optarg;
      break;

    case 'V':
      Pulsar::Archive::set_verbosity (3);
      verbose = true;
      break;

    case 'v':
      Pulsar::Archive::set_verbosity (2);
      verbose = true;
      break;

    case 'q':
      Pulsar::Archive::set_verbosity (0);
      quiet = true;
      break;

    }
  }

  vector <string> filenames;

  if (metafile)
    stringfload (&filenames, metafile);
  else
    for (int ai=optind; ai<argc; ai++)
      dirglob (&filenames, argv[ai]);

  if (filenames.size() == 0) {
    usage ();
    return 0;
  }
  
  Reference::To<Pulsar::Archive> input;
  Reference::To<Pulsar::Archive> output;
  
  for (unsigned ifile=0; ifile < filenames.size(); ifile++) try {

    if (!quiet)
      cerr << "Loading " << filenames[ifile] << endl;
    input = Pulsar::Archive::load(filenames[ifile]);
    
    output = Pulsar::Archive::new_Archive (output_format);
    output-> copy (*input);

    if (!quiet)
      cerr << "Conversion complete" << endl;
    
    if (verbose) {
      cerr << "Source: " << output -> get_source() << endl;
      cerr << "Frequency: " << output -> get_centre_frequency() << endl;
      cerr << "Bandwidth: " << output -> get_bandwidth() << endl;
      cerr << "# of subints: " << output -> get_nsubint() << endl;
      cerr << "# of polns: " << output -> get_npol() << endl;
      cerr << "# of channels: " << output -> get_nchan() << endl;
      cerr << "# of bins: " << output -> get_nbin() << endl;
    }

    string newname = input->get_filename();

    unsigned index = newname.find_last_of(".",newname.length());

    if (index == string::npos)
      index = newname.length();

    newname = newname.substr(0, index);

    if (input->type_is_cal())
      newname += unload_cal_ext;
    else
      newname += unload_psr_ext;

    if (!quiet)
      cerr << "Unloading " << newname << endl;

    output->unload (newname);

  }
  catch (Error& error) {
    cerr << error << endl;
  }
}
