#include <iostream>
#include <stdlib.h>

#include <algorithm>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qmessagebox.h> 
#include <qpopupmenu.h>
#include <qpalette.h>
#include <qplatinumstyle.h>

#include "qt_editParams.h"

#include "rhythm.h"
#include "tempo++.h"

bool Rhythm::verbose = false;
bool Rhythm::vverbose = false;

// /////////////////////////////////////////////////////////////////////
// Main procedure
// /////////////////////////////////////////////////////////////////////

int main (int argc, char** argv)
{ 
  
  try {
    
    QStyle* mystyle = new QPlatinumStyle();
    QPalette mypalette(Qt::darkBlue, Qt::darkCyan);

    QApplication app (argc, argv);

    app.setStyle(mystyle);
    app.setPalette(mypalette);

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
  catch (Error& error) {
    cerr << "rhythm: exception caught:" << error << endl;
  }
  catch (...) {
    cerr << "rhythm: fatal exception unhandled" << endl;
  }
}

// /////////////////////////////////////////////////////////////////////
// Class definitions
// /////////////////////////////////////////////////////////////////////


Rhythm::Rhythm (QWidget* parent, int argc, char** argv) :
  QMainWindow (parent, "Rhythm"),
  opts (0, "Options")
{
  // Initialise variables

  xq = toaPlot::TOA_MJD;
  yq = toaPlot::ResidualMicro;

  toas.resize(0);
  toas_modified = false;

  mode = 1;

  dataPath = ".";

  autofit = false;
  ignore_one_eph = false;
  
  Error::verbose = true;
  setClassVerbose (vverbose);

  // Instantiate a box to hold all the stuff

  QHBox* container = new QHBox(this);
  container -> setFocus();
  setCentralWidget(container);
  
  if (vverbose)
    cerr << "Rhythm:: creating toaPlotter" << endl;
  
  // Instantiate the plotting window

  plot_window = new toaPlot(0,0);
  plot_window -> show();

  QObject::connect(plot_window, SIGNAL(ineednewdata()),
		   this, SLOT(request_update()));

  // Build the cursor control panel

  leftpanel = new QVBox(container);

  QPixmap* pretty_pic = new QPixmap("/home/cluster/ahotan/cvshome/soft_swin/tas/rhythm/banner.jpg");
  header = new QLabel(leftpanel);
  header->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  header->setPixmap(*pretty_pic);

  bottompanel = new QHBox(leftpanel);

  controls = new QVBox(bottompanel);
  
  modechanger = new QButtonGroup(3, Qt::Vertical, "Mode", controls);
  modechanger -> setRadioButtonExclusive(true);
  
  zoom = new QRadioButton("Zoom", modechanger);
  sel = new QRadioButton("Select", modechanger);

  zoom->setChecked(true);
  
  modechanger->insert(zoom,1);
  modechanger->insert(sel,2);
  
  QObject::connect(modechanger, SIGNAL(clicked(int)),
		   this, SLOT(change_mode(int)));

  point = new QPushButton("Point", controls);
  QObject::connect(point, SIGNAL(clicked()),
		   this, SLOT(point_slot()));
  xrange = new QPushButton("X Range", controls);
  QObject::connect(xrange, SIGNAL(clicked()),
		   this, SLOT(xrange_slot()));
  yrange = new QPushButton("Yrange", controls);  
  QObject::connect(yrange, SIGNAL(clicked()),
		   this, SLOT(yrange_slot()));
  box = new QPushButton("Box", controls);
  QObject::connect(box, SIGNAL(clicked()),
		   this, SLOT(box_slot()));
  autoscl = new QPushButton("Scale", controls);
  QObject::connect(autoscl, SIGNAL(clicked()),
		   plot_window, SLOT(autoscale()));
  clearsel = new QPushButton("Clear", controls);
  QObject::connect(clearsel, SIGNAL(clicked()),
		   this, SLOT(clearselection()));
  cut = new QPushButton("Delete", controls);
  QObject::connect(cut, SIGNAL(clicked()),
		   this, SLOT(deleteselection()));
  undel = new QPushButton("Restore", controls);
  QObject::connect(undel, SIGNAL(clicked()),
		   this, SLOT(undeleteall()));

  show_button = new QPushButton("Profile", controls);
  QObject::connect(show_button, SIGNAL(clicked()),
		   this, SLOT(show_me()));
  
  // Instantiate the Axis selection panels

  chooser = new AxisSelector(bottompanel);

  QObject::connect(chooser, SIGNAL(YChange(toaPlot::AxisQuantity)),
		   this, SLOT(YChange(toaPlot::AxisQuantity)));

  QObject::connect(chooser, SIGNAL(XChange(toaPlot::AxisQuantity)),
		   this, SLOT(XChange(toaPlot::AxisQuantity)));

  // Instantiate the toa list box

  toa_text = new QListBox(container, "TOA_INFO");
  toa_text -> setSelectionMode(QListBox::Multi);
  
  toa_text -> setMinimumSize(500,400);

  QObject::connect(toa_text, SIGNAL(selectionChanged()),
		   this, SLOT(reselect()));
  
  QObject::connect(plot_window, SIGNAL(selected(int)),
		   this, SLOT(select(int)));
  
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
    cerr << "Loading TOAs from '" << fname << "'...";
  
  Tempo::toa::load (fname, &toas);
  
  if (verbose)
    cerr << "...loaded " << toas.size() << ".\nSorting...";

  sort (toas.begin(), toas.end());

  if (verbose)
    cerr << " done." << endl;

  tempo->setItemEnabled (fitID, true);
  tempo->setItemEnabled (fitSelID, true);

  toa_filename = fname;
  
  toa_text -> clear();

  char useful[256];

  for (unsigned i = 0; i < toas.size(); i++) {
    toas[i].unload(useful);
    toa_text -> insertItem(useful);
  }
  
  toas_modified = false;

  if (autofit)
    fit ();
}

void Rhythm::save_toas (const char* fname)
{
  if (verbose)
    cerr << "Saving TOAs to '" << fname << "' ...";

  Tempo::toa::unload(fname, toas);

  toa_filename = fname;

  if (verbose)
    cerr << " done." << endl;
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

void Rhythm::close_toas ()
{
  toa_text -> clear();
  toas.resize(0);
  goplot();
}

void Rhythm::show_me ()
{
  int index = toa_text->currentItem();
  
  if (index < 0) {
    QMessageBox::information (NULL, "Rhythm",
			      "There is no selected TOA to plot!  \n");
    return;
  }
  
  if (verbose)
    cerr << "The currently selected TOA is # " << index << endl;

  char useful[80];
  char filename[80];

  toas[index].unload(useful);

  sscanf(useful+1, "%s ", filename);

  if (verbose)
    cerr << "Attempting to load archive '" << filename << "'" << endl;
  
  string useful2 = dataPath + "/";
  useful2 += filename;

  try {
    
    Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);

    data->fscrunch();
    data->tscrunch();
    data->pscrunch();
    data->centre();

    cpgopen("/xs");
    cpgsvp (0.1,0.9,0.1,0.9);
    Pulsar::Plotter plotter;
    plotter.singleProfile(data);
    cpgclos();

  }
  catch (Error& error) {
    if (verbose)
      cerr << "Rhythm::show_me ERROR " << error << endl;
    QMessageBox::critical (this, "Rhythm::show_me",
			   "There you were, enjoying a nice relaxing   \n"
			   "TOA fitting session when suddenly, WHAM!   \n"
			   "    What can I say, that's life...         \n",
			   "Sigh.");
  }
  
}

void Rhythm::fit()
{
  if (!fitpopup || !fitpopup -> hasdata())
    return;

  psrephem eph;
  fitpopup -> get_psrephem (eph);

  fit (eph, true);

  if (verbose)
    cerr << "Rhythm::fit plotting residuals" << endl;

  goplot();
  plot_window->autoscale();

  toa_text -> clear();

  char useful[256];

  for (unsigned i = 0; i < toas.size(); i++) {
    toas[i].unload(useful);
    toa_text -> insertItem(useful);
  }
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
  
} 
 catch (Error& error) {
   if (verbose)
     cerr << "Rhythm::fit ERROR " << error << endl;
   QMessageBox::critical (this, "Rhythm::fit",
			  "They're all out to get you...  ",
			  "Trust no one...");
 }
 catch (...) {
   if (verbose)
     cerr << "Rhythm::fit ERROR Unhandled Exception" << endl;
   QMessageBox::critical (this, "Rhythm::fit",
			  "If you are seeing this it is already too late.   \n"
			  "Abandon all hope...                              \n",
			  "Run Away!");
 }
}

void Rhythm::fit_selected()
{
  if (!fitpopup || !fitpopup -> hasdata())
    return;

  psrephem eph;
  fitpopup -> get_psrephem (eph);

  fit_selected (eph, true);

  fitpopup -> get_psrephem (eph);
  eph.nofit();

  fit (eph, false);

  if (verbose)
    cerr << "Rhythm::fit_selected plotting residuals" << endl;

  goplot();
  
  toa_text -> clear();
  
  char useful[256];

  for (unsigned i = 0; i < toas.size(); i++) {
    toas[i].unload(useful);
    toa_text -> insertItem(useful);
  }
}

void Rhythm::fit_selected (const psrephem& eph, bool load_new)
{ 

  try {
    
    if (toas.size() < 1) {
      if (verbose)
	cerr << "Rhythm::fit_selected No Arrival Times loaded" << endl;
      return;
    }
    
    vector<Tempo::toa> subset;
    
    for (unsigned i = 0; i < toas.size(); i++) {
      if (toas[i].state == Tempo::toa::Selected)
	subset.push_back(toas[i]);
    }
    
    if (subset.size() < 1) {
      if (verbose)
	cerr << "Rhythm::fit_selected No Arrival Times selected" << endl;
      return;
    }
    
    if (verbose)
      cerr << "Rhythm::fit_selected Calculating residuals" << endl;
    
    psrephem pf_eph;
    
    Tempo::fit (eph, subset, &pf_eph, true);
    
    if (load_new && fitpopup) {
      // set_psrephem will result in generation of newEph signal, 
      // which should be ignored since it was set from here.
      ignore_one_eph = true;
      
      if (verbose)
	cerr << "Rhythm::fit_selected Displaying new ephemeris" << endl;
      
      fitpopup -> set_psrephem (pf_eph);
    }
    
  } 
  catch (Error& error) {
    if (verbose)
      cerr << "Rhythm::fit_selected ERROR " << error << endl;
    QMessageBox::critical (this, "Rhythm::fit_selected",
			   "They're all out to get you...  ",
			   "Trust no one...");
  }
  catch (...) {
    if (verbose)
      cerr << "Rhythm::fit_selected ERROR Unhandled Exception" << endl;
    QMessageBox::critical (this, "Rhythm::fit_selected",
			   "If you are seeing this it is already too late.   \n"
			   "Abandon all hope...                              \n",
			   "Run Away!");
  }
}

void Rhythm::setClassVerbose (bool verbose)
{
  qt_editParams::verbose = verbose;
  Tempo::verbose = verbose;
  Tempo::toa::verbose = verbose;
}

vector<double> Rhythm::give_me_data (toaPlot::AxisQuantity q)
{
  vector<double> retval;
  
  switch (q) {
  case toaPlot::TOA_MJD:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back((toas[i].resid.mjd)-50000.0);
    return retval;
    break;
  case toaPlot::BinaryPhase:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(toas[i].resid.binaryphase);
    return retval;
    break;
  case toaPlot::ObsFreq:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(toas[i].resid.obsfreq);
    return retval;
    break;
  case toaPlot::DayOfYear:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(fmod(toas[i].resid.mjd, 365.0));
    return retval;
    break;
  case toaPlot::ResidualMicro:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(toas[i].resid.time);
    return retval;
    break;
  case toaPlot::ResidualMilliTurns:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back((toas[i].resid.turns)*1000.0);
    return retval;
    break;
  case toaPlot::ErrorMicro:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(toas[i].resid.error);
    return retval;
    break;
  case toaPlot::SignalToNoise:
    char useful[80];
    char filename[80];
    for (unsigned i = 0; i < toas.size(); i++) {
      
      toas[i].unload(useful);
      
      sscanf(useful+1, "%s ", filename);
      
      if (verbose)
	cerr << "Attempting to load archive '" << filename << "'" << endl;
      
      string useful2 = dataPath + "/";
      useful2 += filename;
      
      Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
      
      data->fscrunch();
      data->tscrunch();
      data->pscrunch();

      retval.push_back(data->get_Profile(0,0,0)->snr());      
    }
    return retval;
    break;
  default:
    return retval;
    break;
  }
}
 
vector<double> Rhythm::give_me_errs (toaPlot::AxisQuantity q)
{
  vector<double> retval;
  
  // This will require adjustment! Find out what the error
  // is stored as in Willem's residual class...

  switch (q) {
  case toaPlot::TOA_MJD:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
  case toaPlot::ResidualMicro:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(toas[i].resid.error);
    return retval;
    break;
  case toaPlot::ResidualMilliTurns:
    // Need the pulsar period:
    if (fitpopup->hasdata()) {
      psrephem myeph;
      fitpopup -> get_psrephem (myeph);
      double period = myeph.p();
      period *= 1000.0;
      for (unsigned i = 0; i < toas.size(); i++)
	retval.push_back(toas[i].resid.error / period);
      return retval;
    }
    else {
      for (unsigned i = 0; i < toas.size(); i++)
	retval.push_back(0.0);
      return retval;
    }
    break;
  case toaPlot::BinaryPhase:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
  case toaPlot::ObsFreq:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
  case toaPlot::DayOfYear:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
  case toaPlot::ErrorMicro:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
  case toaPlot::SignalToNoise:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;
  default:
    return retval;
    break;
  }
}

void Rhythm::XChange (toaPlot::AxisQuantity q)
{
  xq = q;
  goplot ();
  plot_window->autoscale();
}

void Rhythm::YChange (toaPlot::AxisQuantity q)
{
  yq = q;
  goplot ();
  plot_window->autoscale();
}

void Rhythm::goplot ()
{
  vector<double> tempx = give_me_data(xq);
  vector<double> tempy = give_me_data(yq);
  vector<double> yerrs = give_me_errs(yq);

  vector<wrapper> useme;

  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].state != Tempo::toa::Deleted) {

      wrapper tempw;

      tempw.x = tempx[i];
      tempw.y = tempy[i];
      tempw.e = yerrs[i];
      tempw.id = i;
      if (toas[i].state == Tempo::toa::Selected)
	tempw.ci = 2;

      useme.push_back(tempw);
    }
  }
  
  plot_window->setPoints(xq, yq, useme);
}

void Rhythm::reselect ()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].state == Tempo::toa::Deleted)
      continue;
    
    if (toa_text->isSelected(i))
      toas[i].state = Tempo::toa::Selected;
    else
      toas[i].state = Tempo::toa::Normal;
  }
  
  goplot ();
}

void Rhythm::deselect (int pt)
{
  if (pt >= int(toas.size()))
    return;
  if (pt < 0)
    return;
  
  if (toas[pt].state == Tempo::toa::Deleted)
    return;
  
  toas[pt].state = Tempo::toa::Normal;
  toa_text -> setSelected (pt, false);
  
}

void Rhythm::request_update ()
{
  goplot ();
}

void Rhythm::select (int pt)
{
  if (pt >= int(toas.size()))
    return;
  if (pt < 0)
    return;
  
  if (toas[pt].state == Tempo::toa::Deleted)
    return;
  
  toas[pt].state = Tempo::toa::Selected;
  toa_text -> setSelected (pt, true);
  toa_text -> setCurrentItem (pt);

}

void Rhythm::change_mode (int m)
{
  mode = m;
  cerr << "Mode changed to " << m << endl;
  plot_window->ptselector();
}

void Rhythm::xrange_slot ()
{
  switch (mode) {
  case 1: // Zoom
    plot_window->xzoomer();
    break;
  case 2: // Select
    plot_window->xselector();
    break;
  }
}

void Rhythm::yrange_slot ()
{
  switch (mode) {
  case 1: // Zoom
    plot_window->yzoomer();
    break;
  case 2: // Select
    plot_window->yselector();
    break;
  }
}

void Rhythm::point_slot ()
{
  switch (mode) {
  case 1: // Zoom
    // Do nothing here...
    break;
  case 2: // Select
    plot_window->ptselector();
    break;
  case 3: // Cut
    break;
  }
}

void Rhythm::box_slot ()
{
  switch (mode) {
  case 1: // Zoom
    plot_window->boxzoomer();
    break;
  case 2: // Select
    plot_window->boxselector();
    break;
  }
}

void Rhythm::deleteselection ()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].state == Tempo::toa::Selected)
      toas[i].state = Tempo::toa::Deleted;
  }

  toas_modified = true;

  goplot ();
  plot_window->autoscale();
}

void Rhythm::undeleteall ()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].state == Tempo::toa::Deleted) {
      toas[i].state = Tempo::toa::Normal;
      toa_text -> setSelected (i, false);
    }
  }
  goplot ();
  plot_window->autoscale();
}


void Rhythm::clearselection ()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].state == Tempo::toa::Deleted)
      continue;
    
    toas[i].state = Tempo::toa::Normal;
    toa_text -> setSelected (i, false);
  }
  goplot ();
}

AxisSelector::AxisSelector (QWidget* parent)
  : QHBox(parent)
{
  Xgrp = new QButtonGroup(8, Qt::Vertical, "X Axis", this);
  Xgrp -> setRadioButtonExclusive(true);
  
  Ygrp = new QButtonGroup(8, Qt::Vertical, "Y Axis", this);
  Ygrp -> setRadioButtonExclusive(true);

  X1 = new QRadioButton("Residual (us)", Xgrp);
  X2 = new QRadioButton("Residual (mt)", Xgrp);
  X3 = new QRadioButton("TOA (MJD)", Xgrp);
  X4 = new QRadioButton("Binary Phase", Xgrp);
  X5 = new QRadioButton("Obs Freq", Xgrp);
  X6 = new QRadioButton("Day of Year", Xgrp);
  X7 = new QRadioButton("Timing Error", Xgrp);
  X8 = new QRadioButton("Signal / Noise", Xgrp);

  X3->setChecked(true);

  Y1 = new QRadioButton("Residual (us)", Ygrp);
  Y2 = new QRadioButton("Residual (mt)", Ygrp);
  Y3 = new QRadioButton("TOA (MJD)", Ygrp);
  Y4 = new QRadioButton("Binary Phase", Ygrp);
  Y5 = new QRadioButton("Obs Freq", Ygrp);
  Y6 = new QRadioButton("Day of Year", Ygrp);
  Y7 = new QRadioButton("Timing Error", Ygrp);
  Y8 = new QRadioButton("Signal / Noise", Ygrp);

  Y1->setChecked(true);

  Xgrp->insert(X1,1);
  Xgrp->insert(X2,2);
  Xgrp->insert(X3,3);
  Xgrp->insert(X4,4);
  Xgrp->insert(X5,5);
  Xgrp->insert(X6,6);
  Xgrp->insert(X7,7);
  Xgrp->insert(X8,8);

  Ygrp->insert(Y1,1);
  Ygrp->insert(Y2,2);
  Ygrp->insert(Y3,3);
  Ygrp->insert(Y4,4);
  Ygrp->insert(Y5,5);
  Ygrp->insert(Y6,6);
  Ygrp->insert(Y7,7);
  Ygrp->insert(Y8,8);

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
