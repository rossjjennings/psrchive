#include <iostream>
#include <stdlib.h>

#include <algorithm>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qmessagebox.h> 
#include <qpopupmenu.h>

#include "qt_editParams.h"

#include "rhythm.h"
#include "tempo++.h"

bool Rhythm::verbose = true;
bool Rhythm::vverbose = true;

int main (int argc, char** argv)
{ try {
  
  QApplication app (argc, argv);
  
  Rhythm rhythm (0, argc, argv);
  
  if (Rhythm::vverbose)
    cerr << "call QApplication::setMainWidget" << endl;
  app.setMainWidget (&rhythm);
  
  if (Rhythm::vverbose)
    cerr << "call Rhythm::show" << endl;
  rhythm.show();
  
  if (Rhythm::vverbose)
    cerr << "call QApplication::exec" << endl;
  return app.exec();
}
catch (string error) {
  cerr << "rhythm: exception caught:" << error << endl;
}
catch (char* error) {
  cerr << "rhythm: exception caught:" << error << endl;
}
catch (Error& error) {
  cerr << "rhythm: exception caught:" << error << endl;
}
catch (...) {
  cerr << "rhythm: fatal exception unhandled" << endl;
}
}

Rhythm::Rhythm (QWidget* parent, int argc, char** argv) :
  QMainWindow (parent, "Rhythm"),
  opts (0, "Options")
{
  xq = toaPlot::None;
  yq = toaPlot::None;

  QHBox* container = new QHBox(this);
  container -> setFocus();
  setCentralWidget(container);
  
  if (vverbose)
    cerr << "Rhythm:: creating toaPlotter" << endl;
  plot_window = new toaPlot(0,0);
  plot_window -> show();
  
  controls = new QVBox(container);
  id = new QPushButton("Select", controls);
  QObject::connect(id, SIGNAL(clicked()),
		   plot_window, SLOT(ider()));
  xzoom = new QPushButton("X Zoom", controls);  
  QObject::connect(xzoom, SIGNAL(clicked()),
		   plot_window, SLOT(xzoomer()));
  yzoom = new QPushButton("Y Zoom", controls);
  QObject::connect(yzoom, SIGNAL(clicked()),
		   plot_window, SLOT(yzoomer()));

  toa_text = new QTextEdit(container, "TOA_INFO");
  toa_text -> setText("Welcome to Rhythm");
  toa_text -> setReadOnly(true);
  
  chooser = new AxisSelector(container);

  QObject::connect(chooser, SIGNAL(YChange(toaPlot::AxisQuantity)),
		   this, SLOT(YChange(toaPlot::AxisQuantity)));

  QObject::connect(chooser, SIGNAL(XChange(toaPlot::AxisQuantity)),
		   this, SLOT(XChange(toaPlot::AxisQuantity)));

  
  autofit = true;
  ignore_one_eph = false;
  
  setClassVerbose (vverbose);
  
  if (vverbose)
    cerr << "Rhythm:: new qt_editParams" << endl;
  fitpopup = new qt_editParams;
  connect ( fitpopup, SIGNAL( closed() ),
	    this, SLOT( togledit() ) );
  connect ( fitpopup, SIGNAL( newParams(const psrephem&) ),
	    this, SLOT( set_Params(const psrephem&) ) );

  if (vverbose)
    cerr << "Rhythm:: call menubarConstruct" << endl;
  menubarConstruct(); 

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
  
  Tempo::toa::load (fname, &toas);
  
  if (verbose)
    cerr << "  loaded " << toas.size() << ".\nSorting TOAS...";

  sort (toas.begin(), toas.end());

  if (verbose)
    cerr << "  sorted." << endl;

  tempo->setItemEnabled (fitID, true);

  toa_filename = fname;
  
  char useful[256];
  string output;
  for (unsigned i = 0; i < toas.size(); i++) {
    toas[i].unload(useful);
    output += useful;
    output += "\n";
  }
  
  toa_text -> setText(output.c_str());

  if (autofit)
    fit ();
}

void Rhythm::set_Params (const psrephem& eph)
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

  psrephem eph;
  fitpopup -> get_psrephem (eph);

  fit (eph, true);
}

void Rhythm::fit (const psrephem& eph, bool load_new)
{ try {
    
  if (toas.size() < 1) {
    if (verbose)
      cerr << "Rhythm::fit No Arrival Times loaded" << endl;
    return;
  }

  if (verbose)
    cerr << "Rhythm::fit Calculating residuals" << endl;
  
  psrephem pf_eph;
  
  Tempo::fit (eph, toas, &pf_eph, true);
  
  if (load_new && fitpopup) {
    // set_psrephem will result in generation of newEph signal, 
    // which should be ignored since it was set from here.
    ignore_one_eph = true;
    
    if (verbose)
      cerr << "Rhythm::fit Displaying new ephemeris" << endl;
    
    fitpopup -> set_psrephem (pf_eph);
  }
  
  char useful[256];
  string output;
  
  for (unsigned i = 0; i < toas.size(); i++) {
    toas[i].unload(useful);
    output += useful;
    output += "   RESIDUAL (us) :   ";
    sprintf(useful, "%f", toas[i].resid.time);
    output += useful;
    output += "\n";
  }

  toa_text -> setText(output);
  
  if (verbose)
    cerr << "Rhythm::fit plotting residuals" << endl;
  
  xq = toaPlot::TOA_MJD;
  yq = toaPlot::ResidualMicro;
  goplot ();
  
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
  qt_editParams::verbose = verbose;
  Tempo::verbose = verbose;
}

vector<double> Rhythm::give_me_this (toaPlot::AxisQuantity q)
{
  vector<double> retval;
  
  switch (q) {
  case toaPlot::TOA_MJD:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back((toas[i].resid.mjd)-50000.0);
    return retval;
  case toaPlot::ResidualMicro:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(toas[i].resid.time);
    return retval;
  case toaPlot::ResidualMilliTurns:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back((toas[i].resid.turns)/1000.0);
    return retval;
  default:
    return retval;
  }
}
 
void Rhythm::XChange (toaPlot::AxisQuantity q)
{
  xq = q;
  goplot ();
}

void Rhythm::YChange (toaPlot::AxisQuantity q)
{
  yq = q;
  goplot ();
}

void Rhythm::goplot ()
{
  plot_window->setPoints(xq,give_me_this(xq), 
			 yq, give_me_this(yq));
}

AxisSelector::AxisSelector (QWidget* parent)
  : QHBox(parent)
{
  Xgrp = new QButtonGroup(3, Qt::Vertical, "X Axis", this);
  Xgrp -> setRadioButtonExclusive(true);
  
  Ygrp = new QButtonGroup(3, Qt::Vertical, "Y Axis", this);
  Ygrp -> setRadioButtonExclusive(true);

  X1 = new QRadioButton("Res(us)", Xgrp);
  X2 = new QRadioButton("Res(mt)", Xgrp);
  X3 = new QRadioButton("TOA(MJD)", Xgrp);

  Y1 = new QRadioButton("Res(us)", Ygrp);
  Y2 = new QRadioButton("Res(mt)", Ygrp);
  Y3 = new QRadioButton("TOA(MJD)", Ygrp);

  Xgrp->insert(X1,1);
  Xgrp->insert(X2,2);
  Xgrp->insert(X3,3);

  Ygrp->insert(Y1,1);
  Ygrp->insert(Y2,2);
  Ygrp->insert(Y3,3);

  QObject::connect(Xgrp, SIGNAL(clicked(int)),
		   this, SLOT(Xuseful(int)));
  
  QObject::connect(Ygrp, SIGNAL(clicked(int)),
		   this, SLOT(Yuseful(int)));
}

void AxisSelector::Xuseful(int placeholder)
{
  emit XChange(toaPlot::AxisQuantity(placeholder));
}

void AxisSelector::Yuseful(int placeholder)
{
  emit YChange(toaPlot::AxisQuantity(placeholder));
}
