#include "Predict.h"

#include "tempo++.h"
#include "psrephem.h"
#include "poly.h"

#include "Error.h"
#include "string_utils.h"
#include "genutil.h"

#include <unistd.h>
#include <errno.h>

Tempo::Predict::Predict ()
{
  nspan  = 960;
  ncoef = 12;
  maxha  = 8;
  asite  = '7';
  frequency = 1400.0;

  verify = true;
}

void Tempo::Predict::set_asite (char code)
{
  if (code < 10)
    code += '0';

  asite = code;
}

void Tempo::Predict::set_frequency (double MHz)
{
  frequency = MHz;
}

void Tempo::Predict::set_maxha (unsigned hours)
{
  maxha = hours;
}

void Tempo::Predict::set_nspan (unsigned minutes)
{
  nspan = minutes;
}

void Tempo::Predict::set_ncoef (unsigned _ncoef)
{
  ncoef = _ncoef;
}

void Tempo::Predict::set_parameters (const psrephem& _parameters)
{
  parameters = _parameters;

  psrname = parameters.psrname();

  if (psrname.empty())
    throw Error (InvalidParam, "Tempo::predict::set_parameters", 
		 "psrephem::psrname is empty");

  if (psrname[0] == 'J')
    psrname.erase (0,1);
}

void Tempo::Predict::set_verify (bool _verify)
{
  verify = _verify;
}

// ******************************************************************

/*! The file tz.in will be written in the directory returned by 
    Tempo::get_directory, according to the following format:

     TZ.IN FORMAT

     The tz.in file is a free-format, ASCII file.  Items on a line may be
     separated by any number of spaces and tabs.  
     
     The first line contains default parameters:
     ASITE       a one-character site code (as in TOA LINES, above)
     MAXHADEF    default maximum hour angle for observations (hr)
     NSPANDEF    default time span per ephemeris entry (minutes)
     NCOEFFDEF   default number of coefficients per ephemeris entry
     FREQDEF     default observing frequency (MHz)
    
*/
void Tempo::Predict::write_tzin () const
{
  if (psrname.empty())
    throw Error (InvalidState, "Tempo::Predict::write_tzin",
		 "psrname is empty");


  string tzin = get_directory() + "/tz.in";

  if (Tempo::verbose)
    cerr << "Tempo::Predict::write_tzin to " << tzin << endl;
    
  FILE* fptr = fopen (tzin.c_str(), "w");
  if (!fptr)
    throw Error (FailedSys, "Tempo::Predict::write_tzin",
		 "ERROR fopen (" + tzin + ")");

  fprintf (fptr,"%c %d %d %d %8g\n", 
	   asite, maxha, nspan, ncoef, frequency);

  fprintf(fptr,"\n");
  fprintf(fptr,"\n");
  fprintf(fptr,"%s\n", psrname.c_str());

  fclose(fptr);
}


//! Returns a polyco valid over the range in MJD specified by m1 and m2
polyco Tempo::Predict::get_polyco (const MJD& m1, const MJD& m2) const
{  
  MJD to_tempo_m1 = m1;
  MJD to_tempo_m2 = m2;

  MJD half_day (0, 0.5);

  if (m2-m1 < 2*half_day)  {
    to_tempo_m1 -= half_day;
    to_tempo_m2 += half_day;
  }

  bool satisfied = false;
  
  // start with a clean working directory
  removedir (get_directory().c_str());

  while (!satisfied) {
    
    /* TEMPO will often return a polyco that does not span the range of
       MJDs requested (even when you give it half a day of forgiveness on
       either end)...  so: increase the range until TEMPO gets it right
       and satisfies the requirements of an otherwise frustrated end-user */

    write_tzin();

    string ephname = get_directory() + "/" + ephem_filename;

    if (Tempo::verbose)
      cerr << "Tempo::predict writing " << ephname << endl;
    parameters.unload (ephname);
    
    string arguments = " -z -f " + ephem_filename;
    string input;

    if (m1 == unspecified)
      input = "\n";
    else
      input = stringprintf (" %g %g\n",
			    to_tempo_m1.in_days(), to_tempo_m2.in_days());
    if (Tempo::verbose)
      cerr << "Tempo::predict calling 'tempo " << arguments << "'" << endl
	   << "Tempo::predict input: '" << input << "'" << endl;
    
    tempo (arguments, input);

    string stderr_name = get_directory() + "/" + stderr_filename;

    bool error = false;

    // check for error and/or warning messages
    FILE* tempo_err = fopen (stderr_name.c_str(), "r");
    if (tempo_err) {
      char* inbuf = new char[80];
      while (fgets (inbuf, 80, tempo_err) != NULL) {
	if (!error)
	  fprintf (stderr, "Tempo::predict Errors/Warnings detected:\n");
	error = true;
	fprintf (stderr, inbuf);
      }
      fclose (tempo_err);
      delete [] inbuf;
    }

    if (error)
      throw Error (FailedSys, "Tempo::predict", "TEMPO Warnings detected");

    string polyco_dat = get_directory() + "/polyco.dat";

    if (Tempo::verbose)  {
      fprintf (stderr, "Tempo::predict loading polyco.dat\n");
      fprintf (stderr, "****** BEGINNING OF FILE ******\n");
      system( ("cat " + polyco_dat).c_str() );
      fprintf (stderr, "********* END OF FILE *********\n");
    }

    polyco poly;

    if (poly.load(polyco_dat) < 1)
      throw Error (FailedSys, "Tempo::predict",
		   "failed polyco::load(" + polyco_dat + ")");

    if (verbose)
      cerr << "Tempo::predict scanned " << poly.pollys.size() 
	   << " polynomials" << endl;
    
    if (!verify || m1 == unspecified)
      // no need for a validity check
      return poly;
    
    // a simple validity check
    if ( poly.start_time() > m1 || poly.end_time() < m2  ) {

      if (verbose)
	cerr << "Tempo::predict insufficient span: " <<
	  poly.start_time().in_days() << " -> " << poly.end_time().in_days() 
	     << "\n   requested: " << m1.in_days() << " -> " << m2.in_days()
	     << endl;
      
      if (poly.start_time() > m1)
	to_tempo_m1 -= half_day;

      if (poly.end_time() < m2)
	to_tempo_m2 += half_day;

      if (verbose)
	cerr << "Tempo::predict retry: " << to_tempo_m1.in_days() 
	     << " -> " << to_tempo_m2.in_days() << endl;
    }

    else {  // polyco OK

      if (Tempo::verbose) {
	fprintf (stderr, 
		 "Tempo::predict:: polyco span OK.\n");
	fprintf (stderr, "span: %g->%g\n",
		 poly.start_time().in_days(), poly.end_time().in_days());
	
	fprintf (stderr, "reqd: %g->%g\n", m1.in_days(), m2.in_days());
      }

      return poly;

    }  // end else polyco OK
  }  // end while not satisfied

  throw Error (InvalidState, "Tempo::Predict::get_polyco", "no polyco");

}

