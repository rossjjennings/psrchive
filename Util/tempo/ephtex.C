#include <iostream>
#include "psrephem.h"

int main (int argc, char** argv)
{
  bool aastex = false;
  bool nature = false;
  bool verbose = false;

  int c;
  while ((c = getopt(argc, argv, "hanv")) != -1) {
    switch (c)  {
    case 'h':
      cerr <<
	"ephtex - produce a LaTeX formatted table of pulsar parameters\n"
	"\n"
	"USAGE:  ephtex [hanv] filename\n"
	"\n"
	" -h    this help\n"
	" -a    aastex deluxe table\n"
	" -n    Nature suitable\n"
	"       (see http://www.nature.com/nature/submit/gta/index.html#4.6\n"
	" -v    verbose on stderr\n"
	   << endl;
      return 0;

    case 'a':
      aastex = true;
      break;

    case 'n':
      nature = true;
      break;

    case 'v':
      verbose = true;
      break;
    }
  }

  if (optind == argc)
    cerr << "ephtex: provide a TEMPO parameter file as the last argument"
	 << endl;
  
  psrephem eph (argv[1]);

  if (aastex) {
    cout <<
      "\\begin{deluxetable}{lr}\n"
      "\\tablecaption{Derived Parameters for " << eph.tex_name() << "}\n"
      "\\tablecolumns{1}\n"
      "\\tablewidth{0pt}\n"
      "\\startdata\n"
	 << endl;
  }
  else {
    cout <<
      "\\begin{table}\n"
      "\\begin{tabular}{lr}\n"
      "\\multicolumn{2}{c}{\bf Derived Parameters for " 
	 << eph.tex_name() << "} \\\\"
	 << endl;

    if (!nature)
      cout << " \\hline \\hline" << endl;
  }

  cout << eph.tex() << endl;

  if (aastex) {
    cout <<
      "\\enddata\n"
      "\\end{deluxetable}\n"
	 << endl;
  }
  else {
    cout <<
      "\\end{tabular}\n"
      "\\end{table}\n"
	 << endl;
  }
}

