#include <iostream>
#include <stdlib.h>

#include <algorithm>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qmessagebox.h> 
#include <qpopupmenu.h>
#include <qxt.h>

#include "qt_editParams.h"
#include "qt_ModelOptions.h"

#include "rhythm.h"
#include "tempo++.h"
#include "DataManager.h"
#include "PlotVolume.h"
#include "PlotManager.h"

bool Rhythm::verbose = true;
bool Rhythm::vverbose = true;

int main (int argc, char** argv)
{ try {

  QXtApplication app (argc, argv, "RhythmApp");
  Rhythm rhythm (0, argc, argv);

  if (Rhythm::vverbose)
    cerr << "call QXtApplication::setMainWidget" << endl;
  app.setMainWidget (&rhythm);

  if (Rhythm::vverbose)
    cerr << "call Rhythm::show" << endl;
  rhythm.show();


  if (Rhythm::vverbose)
    cerr << "call QXtApplication::exec" << endl;
  return app.exec();
}
catch (string error) {
  cerr << "rhythm: Fatal exception caught:" << error << endl;
}
catch (char* error) {
  cerr << "rhythm: Fatal exception caught:" << error << endl;
}
catch (Reference::invalid) {
  cerr << "rhythm: Fatal exception caught: Reference::invalid" << endl;
}
catch (...) {
  cerr << "rhythm: Fatal exception unhandled" << endl;
}
}

Rhythm::Rhythm (QWidget* parent, int argc, char** argv) :
  QMainWindow (parent, "Rhythm"),
  opts (0, "Options")
{
  autofit = true;
  ignore_one_eph = false;

  setClassVerbose (vverbose);
  initializePlot ();

  if (data_manager.size() > 0) {
    if (vverbose)
      cerr << "Rhythm:: first data_manager manage model" << endl;
    data_manager[0]->manage (modelPlot);
    setCentralWidget (plot_manager[0]);
  }

  if (vverbose)
    cerr << "Rhythm:: data set model" << endl;
  modelPlot.setModel(model);

  opts.modelOptions -> setModel(modelPlot);

  if (vverbose)
    cerr << "Rhythm:: new qt_editParams" << endl;
  fitpopup = new qt_editParams;
  connect ( fitpopup, SIGNAL( closed() ),
	    this, SLOT( togledit() ) );
  connect ( fitpopup, SIGNAL( newParams(const psrParams&) ),
	    this, SLOT( set_Params(const psrParams&) ) );

  if (vverbose)
    cerr << "Rhythm:: call menubarConstruct" << endl;
  menubarConstruct(); 
  // toolbarConstruct(); 

  if (vverbose)
    cerr << "Rhythm:: call command_line_parse" << endl;
  command_line_parse (argc, argv);

  if (vverbose)
    cerr << "Rhythm:: show qt_editParams" << endl;
  fitpopup -> show();
}

void Rhythm::load_toas (const char* fname)
{
  if (verbose)
    cerr << "Loading TOAs from '" << fname << "' ...";

  model.load (fname);

  if (verbose)
    cerr << "  loaded " << model.toas.size() << ".\nSorting TOAS...";

  sort (model.toas.begin(), model.toas.end());

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
    
  if (model.toas.size() < 1) {
    if (verbose)
      cerr << "Rhythm::fit No Arrival Times loaded" << endl;
    return;
  }

  if (verbose)
    cerr << "Rhythm::fit Calculating residuals" << endl;
  
  Tempo::fit (eph, model.toas, &model.eph, true);
  
  if (load_new && fitpopup) {
    // set_psrParams will result in generation of newEph signal, which should
    // be ignored since it was set from here.

    if (verbose)
      cerr << "Rhythm::fit Displaying new ephemeris" << endl;

    ignore_one_eph = true;
    fitpopup -> set_psrParams (model.eph);
  }
  
  if (verbose)
    cerr << "Rhythm::fit plotting residuals" << endl;
  modelPlot.setModel (model);
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

void Rhythm::setClassVerbose (bool verbose)
{
  // qt_fileParams::verbose = verbose;
  qt_editParams::verbose = verbose;
  Tempo::verbose = verbose;
  Tempo::ModelDataSet::verbose = verbose;
  Plot::Volume::verbose = verbose;
  Plot::Manager::verbose = verbose;
  DataManager::verbose = verbose;
}
