#include <stdio.h>

#include "resio.h"
#include "residual.h"

int residual::verbose = 0;

void residual::init ()
{
  mjd = turns = seconds = binaryphase = obsfreq 
    = weight = error = preres = 0.0;
}

int residual::load (int lun)
{
  int status = 0;
  obsfreq = -20.0;
  resread_ (&lun, &mjd, &turns, &seconds, &binaryphase,
	    &obsfreq, &weight, &error, &preres, &status);
  return status;
}

// ////////////////////////////////////////////////////////////////////////
// residual::load
//
// Loads residual objects from the fortran binary file produced by TEMPO
// (usually resid2.tmp).
//
// r2flun    - fortran logical unit number with which to open the file
// filename  - name of the file
// residuals - vector of residual objects into which .size() residuals
//             will be loaded
// ////////////////////////////////////////////////////////////////////////
int residual::load (int r2flun, char* filename, vector<residual>* residuals)
{
  resopen_ (&r2flun, filename, (int) strlen(filename));
  if (verbose)
    fprintf (stderr, "residual::load (vector<residual>) '%s' opened\n", 
	     filename);

  residuals->erase (residuals->begin(), residuals->end());
  residual res_rd;

  while (res_rd.load (r2flun) == 0)
    residuals->push_back (res_rd);

  resclose_ (&r2flun);
  return 0;
}

residual::plot residual::xtype = Mjd;
residual::plot residual::ytype = Seconds;

float y_ordinate (const residual& r)
{
  switch (residual::ytype)  {
  case residual::Seconds:
    return r.seconds;
  case residual::Turns:
    return r.turns;
  default:
    return -1.0;
  }
}

float x_ordinate (const residual& r)
{
  switch (residual::xtype)  {
  case residual::Mjd:
    return r.mjd;
  case residual::BinaryPhase:
    return r.binaryphase;
  default:
    return -1.0;
  }
}
