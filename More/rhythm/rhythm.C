#include <iostream>
#include <stdlib.h>

#include <algorithm>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qxt.h>

#include "qt_editParams.h"
#include "rhythm.h"

bool Rhythm::verbose = true;

int main (int argc, char** argv)
{
  QXtApplication app (argc, argv, "RhythmApp");
  Rhythm rhythm (0, argc, argv);
  
  app.setMainWidget (&rhythm);
  rhythm.show();
  return app.exec();
}

Rhythm::Rhythm (QWidget* parent, int argc, char** argv) :
  QMainWindow (parent, "Rhythm"),
  plot_manager (this, 800, 700)
{
  autofit = true;
  ignore_one_eph = false;
    
  fitpopup = new qt_editParams;
  connect ( fitpopup, SIGNAL( closed() ),
	    this, SLOT( togledit() ) );
  connect ( fitpopup, SIGNAL( newParams(const psrParams&) ),
	    this, SLOT( set_Params(const psrParams&) ) );

  menubarConstruct(); 

  setCentralWidget (&plot_manager);

  command_line_parse (argc, argv);

  fitpopup -> show();
}

void Rhythm::load_toas (const char* fname)
{
  if (verbose)
    cerr << "Loading TOAs from '" << fname << "' ...";

  if (toa::load (tim_filename.c_str(), &arrival_times) < 0) {
    cerr << "Error!" << endl;
    return;
  }

  if (verbose)
    cerr << "  loaded" << endl << "Sorting TOAS...";

  sort (arrival_times.begin(), arrival_times.end());

  if (verbose)
    cerr << "  sorted" << endl;

  tempo->setItemEnabled (fitID, true);

  if (autofit)
    fit ();
}

void Rhythm::set_Params (const psrParams& eph)
{
  if (ignore_one_eph) {
    ignore_one_eph = false;
    return;
  }
  if (autofit)
    fit (eph, false);
}

void Rhythm::fit()
{
  if (!fitpopup || !fitpopup -> hasdata())
    return;

  psrParams eph;
  fitpopup -> get_psrParams (&eph);

  fit (eph, true);
}

void Rhythm::fit (const psrParams& eph, bool load_new)
{
  if (arrival_times.size() < 1) {
    if (verbose)
      cerr << "Rhythm::fit No Arrival Times loaded" << endl;
    return;
  }

  if (verbose)
    cerr << "Rhythm::fit Calculating residuals" << endl;
  
  psrParams neweph;
  tempo_fit (eph, arrival_times, &neweph, &residuals);

  if (load_new && fitpopup) {
    // set_psrParams will result in generation of newEph signal, which should
    // be ignored since it was set from here.
    ignore_one_eph = true;
    fitpopup -> set_psrParams (neweph);
  }
}
