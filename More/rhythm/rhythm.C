#include <iostream>
#include <stdlib.h>

#include <algorithm>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qmessagebox.h> 
#include <qpopupmenu.h>
#include <qxt.h>

#include "qt_editParams.h"
#include "rhythm.h"
#include "tempo++.h"

bool Rhythm::verbose = false;
bool Rhythm::vverbose = false;

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
  opts (0, "Options"),
  plot_manager (this, 800, 700),
  res_plot (pg_point(0.049,0.089),pg_point(0.98,0.98))
{
  autofit = true;
  ignore_one_eph = false;

  plot_manager.manage (&res_plot);
  labelPlot ();

  fitpopup = new qt_editParams;
  connect ( fitpopup, SIGNAL( closed() ),
	    this, SLOT( togledit() ) );
  connect ( fitpopup, SIGNAL( newParams(const psrParams&) ),
	    this, SLOT( set_Params(const psrParams&) ) );

  menubarConstruct(); 
  // toolbarConstruct(); 

  setCentralWidget (&plot_manager);

  command_line_parse (argc, argv);

  fitpopup -> show();
}

void Rhythm::load_toas (const char* fname)
{
  if (verbose)
    cerr << "Loading TOAs from '" << fname << "' ...";

  if (toa::load (fname, &arrival_times) < 0) {
    cerr << "Error!" << endl;
    return;
  }

  if (verbose)
    cerr << "  loaded." << endl << "Sorting TOAS...";

  sort (arrival_times.begin(), arrival_times.end());

  if (verbose)
    cerr << "  sorted." << endl;

  tempo->setItemEnabled (fitID, true);

  if (autofit)
    fit ();
}

void Rhythm::set_Params (const psrParams& eph)
{
  tempo->setItemEnabled (saveParmsID, true);

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
{ try {
    
  if (arrival_times.size() < 1) {
    if (verbose)
      cerr << "Rhythm::fit No Arrival Times loaded" << endl;
    return;
  }

  if (!res_plot.empty()) {
    // fit only for the points that are in sight
    for (unsigned ipt=0; ipt<arrival_times.size(); ipt++)
      arrival_times[ipt].set_selected(res_plot.getMostSelect(ipt));
  }

  if (verbose)
    cerr << "Rhythm::fit Calculating residuals" << endl;
  
  psrParams neweph;
  Tempo::fit (eph, arrival_times, &neweph, &residuals);
  
  if (load_new && fitpopup) {
    // set_psrParams will result in generation of newEph signal, which should
    // be ignored since it was set from here.

    if (verbose)
      cerr << "Rhythm::fit Displaying new ephemeris" << endl;

    ignore_one_eph = true;
    fitpopup -> set_psrParams (neweph);
  }
  
  if (verbose)
    cerr << "Rhythm::fit plotting residuals" << endl;

  if (res_plot.empty()) {
    res_plot.load_points  (residuals, false);  // load points, no range calc
    res_plot.load_yerrors (residuals, false);  // load errors, no range calc
    res_plot.auto_range   (false);             // calc range, do not force
  }
  else {
    unsigned iresid = 0;
    for (unsigned ipt=0; ipt<arrival_times.size(); ipt++) {
      if (arrival_times[ipt].is_selected()) {
	res_plot.set_point (ipt, residuals[iresid]);
	res_plot.set_yerror (ipt, residuals[iresid]);
	iresid ++;
      }
    }
  }
  if (plot_manager.isVisible())
    plot_manager.pgplot();
} 
 catch (string error) {
   if (verbose)
     cerr << "Rhythm::fit ERROR " << error << endl;
   QMessageBox::critical (this, "Rhythm::fit",
			  error.c_str(), "Dismiss");
 }
 catch (...) {
   if (verbose)
     cerr << "Rhythm::fit ERROR Unhandled Exception" << endl;
   QMessageBox::critical (this, "Rhythm::fit",
			  "An Unhandled Exception Occured", "Dismiss");
 }
}

void Rhythm::labelPlot()
{
  switch (residual::ytype)  {
  case residual::Seconds:
    res_plot.set_ylabel ("Residual (seconds)");
    break;
  case residual::Turns:
    res_plot.set_ylabel ("Residual (turns)");
    break;
  default:
    cerr << "Rhythm::labelPlot unhandled case for y-axis" << endl;
    break;
  }

  switch (residual::xtype)  {
  case residual::Mjd:
    res_plot.set_xlabel ("MJD");
    break;
  case residual::BinaryPhase:
    res_plot.set_xlabel ("Binary Phase");
    break;
  default:
    cerr << "Rhythm::labelPlot unhandled case for x-axis" << endl;
    break;
  }
}
