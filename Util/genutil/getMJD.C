/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "MJD.h"

#include <iostream>
using namespace std;

int main (int argc, char* argv[]) 
{
  const char* format = "%Y-%m-%d-%H:%M:%S";

  MJD  mjd;
  char printhere[40];
  bool verbose = false;
  bool mjdgiven = false;
  string use_strptime;
  
  int c;
  while ((c = getopt(argc, argv, "d:f:hm:v")) != -1)
  {
    switch (c)
    {
    case 'h':
      cerr <<
	"getMJD [date]\n"
	"WHERE: date is a UTC of the form yyyy-ddd-hh:mm:ss \n"
	"       date is optional. [default: now] \n"
	"ALSO:  getMJD -m mjd [-f format] \n"
	"       prints the date of given MJD. \n"
	"ALSO:  getMJD -d date [-f format] \n"
	"       prints the MJD of date of the form specified by format \n"
	"\n"
	"format is an optional strftime format string"
	" [default:" << format << "] \n"
	   << endl;
      return 0;

    case 'd':
      use_strptime = optarg;
      break;
      
    case 'f':
      format = optarg;
      break;

    case 'm':
      if (verbose)
	cerr << "getMJD: parse MJD from '" << optarg << "'" << endl;
      mjd.Construct (optarg);
      mjdgiven = true;
      break;

    case 'v':
      verbose = true;
      break;
    }
  }

  if (mjdgiven)
  {
    struct tm date;
    double fracsec;
    mjd.gregorian (&date, &fracsec);
    cerr << "Date for MJD given: " << asctime(&date) << endl;
    cout << mjd.datestr(format) << endl;
    return 0;
  }

  if (!use_strptime.empty())
  {
    if (verbose)
      cerr << "Converting '" << use_strptime << "' to date" << endl;

    struct tm time;
    if ( strptime (use_strptime.c_str(), format, &time) == NULL )
    {
      cerr << "Error parsing '" << use_strptime
	   << "' using '" << format << "'" << endl;
      return -1;
    }

    mjd = MJD (time);
  }
  
  else if (optind < argc)
  {
    if (verbose)
      cerr << "Converting '" << argv[optind] << "' to UTC and using as date"
	   << endl;

    utc_t utcdate;
    if (str2utc (&utcdate, argv[optind]) < 0)
      {
      cerr << "Error converting '" << argv[optind] << "' to UTC." << endl;
      return -1;
    }

    cerr << "Using UTC parsed: "
	 << utc2str (printhere, utcdate, "yyyy-ddd-hh:mm:ss") << endl;

    mjd = MJD (utcdate);
  }
  else
  {
    time_t temp = time(NULL);
    struct tm date = *gmtime(&temp);
    cerr << "Using today's date: " << asctime(&date) << endl;
    mjd = MJD (date);
  }

  cout << mjd.printdays(10) << endl;

  if (verbose)
  {
    utc_t  stuffback;
    mjd.UTC (&stuffback);
    cerr << "utc from MJD: "
	 << utc2str (printhere, stuffback, "yyyy-ddd-hh:mm:ss") << endl;
  }

  return 0;
}


