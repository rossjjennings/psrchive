#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "tempo++.h"
#include "toa.h"
#include "residual.h"
#include "psrParams.h"

// ////////////////////////////////////////////////////////////////////////
// Tempo::fit
//
// runs TEMPO on a vector of toa objects, using a given psrParams object.
// returns a vector of residual objects, along with a new "post-fit"
// psrParams object.
//
// model     - input TEMPO-style PSR ephemeris
// toas      - input toas
// postfit   - new PSR ephemeris output by TEMPO
// residuals - vector of residual objects into which tempo output will 
//             be loaded
// ////////////////////////////////////////////////////////////////////////

bool Tempo::verbose = false;
MJD  Tempo::unspecified;

void Tempo::fit (const psrParams& model, const vector<toa>& toas,
		 psrParams* postfit, vector<residual>* residuals)
{
  char* tempo_tim = "arrival.tim";
  char* tempo_par = "arrival.par";
  char* tempo_res = "resid2.tmp";
  
  int   r2flun = 32;

  if (verbose)
    cerr << "Tempo::fit writing TOAs to '" << tempo_tim << "'" << endl;

  // unload the toas into a temporary file
  FILE* fptr = fopen (tempo_tim, "w");
  if (fptr==NULL) {
    fprintf (stderr, "fit error opening %s:\n",tempo_tim);
    perror (":");
    throw ("fit() cannot open file");
  }
  int unloaded = 0;
  for (unsigned iarr=0; iarr < toas.size(); iarr++)  {
    if (!toas[iarr].is_deleted() && toas[iarr].is_selected()) {
      toas[iarr].Tempo_unload (fptr);
      unloaded ++;
    }
  }
  fclose (fptr); 

  // unload the ephemeris
  if (verbose)
    cerr << "Tempo::fit writing parameters to '" << tempo_par << "'" << endl;
  model.unload(tempo_par);

  // run TEMPO
  string runtempo ("tempo ");
  runtempo += tempo_tim;

  if (verbose)
    cerr << "Tempo::fit system (" << runtempo << ")" << endl;

  int retval = system (runtempo.c_str());
  if (retval != 0)  {
    cerr << "Tempo::fit ERROR system (\"" << runtempo 
	 << "\") returns " << retval;
    if (retval < 0)
      perror (" ");
    else
      cerr << ":: tempo returns:" << WIFEXITED(retval) << endl;
    throw (string ("Tempo::fit"));
  }

  if (postfit) {
    // load the new ephemeris (PSRNAME.par in current working directory)
    char* dotpar = ".par";
    string tpopar = model.psrname() + dotpar;
    postfit->load( tpopar.c_str() );
  }

  if (residuals) {
    // load the residuals from resid2.tmp
    residuals->resize(unloaded);
    residual::load (r2flun, tempo_res, residuals);
  }
}
