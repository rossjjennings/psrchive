#include <iostream>
#include <stdlib.h>

#include <algorithm>

#include <qmainwindow.h>
#include <qmessagebox.h> 
#include <qpopupmenu.h>
#include <qpalette.h>
#include <qplatinumstyle.h>
#include <qfont.h>

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

    Rhythm rhythm (&app, 0, argc, argv);
    
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


Rhythm::Rhythm (QApplication* master, QWidget* parent, int argc, char** argv) :
  QMainWindow (parent, "Rhythm"),
  opts (0, "Options")
{
  // Initialise variables

  myapp = master;

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
  
  string banloc = getenv("PSRHOME");
  banloc += "/runtime/rhythm/banner.jpg";

  QPixmap* pretty_pic = new QPixmap(banloc.c_str());
  header = new QLabel(leftpanel);
  header->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  header->setPixmap(*pretty_pic);

  footer = new QLabel("Status Message Box", leftpanel);
  footer->setMinimumHeight(30);
  footer->setAlignment(Qt::AlignCenter);
  footer->setFont (QFont( "Times", 10, QFont::Bold));

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
  xrange = new QPushButton("X-Range", controls);
  QObject::connect(xrange, SIGNAL(clicked()),
		   this, SLOT(xrange_slot()));
  yrange = new QPushButton("Y-Range", controls);  
  QObject::connect(yrange, SIGNAL(clicked()),
		   this, SLOT(yrange_slot()));
  box = new QPushButton("Box", controls);
  QObject::connect(box, SIGNAL(clicked()),
		   this, SLOT(box_slot()));
  autoscl = new QPushButton("Autoscale Axes", controls);
  QObject::connect(autoscl, SIGNAL(clicked()),
		   plot_window, SLOT(autoscale()));
  clearsel = new QPushButton("Clear Selected", controls);
  QObject::connect(clearsel, SIGNAL(clicked()),
		   this, SLOT(clearselection()));
  cut = new QPushButton("Delete Selected", controls);
  QObject::connect(cut, SIGNAL(clicked()),
		   this, SLOT(deleteselection()));
  undel = new QPushButton("Restore Deleted", controls);
  QObject::connect(undel, SIGNAL(clicked()),
		   this, SLOT(undeleteall()));
  show_button = new QPushButton("Show Profile", controls);
  QObject::connect(show_button, SIGNAL(clicked()),
		   this, SLOT(show_me()));
  freqs = new QPushButton("Freq Sort", controls);
  QObject::connect(freqs, SIGNAL(clicked()),
		   this, SLOT(freqsort()));
  colour = new QPushButton("Change Colour", controls);
  QObject::connect(colour, SIGNAL(clicked()),
		   this, SLOT(colour_selector()));
  dotify = new QPushButton("Change Symbol", controls);
  QObject::connect(dotify, SIGNAL(clicked()),
		   this, SLOT(symbol_selector()));
  

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

  QObject::connect(plot_window, SIGNAL(selected(vector<int>)),
		   this, SLOT(select(vector<int>)));
  
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
    cerr << "Loading TOAs from '" << fname << "'";
  
  Tempo::toa::load (fname, &toas);
  
  char num[8];
  sprintf(num, "%d", toas.size());
  
  QString str = "Loaded ";
  str += num;
  str += " TOA's";
  
  footer->setText(str);

  tempo->setItemEnabled (fitID, true);
  tempo->setItemEnabled (fitSelID, true);

  toa_filename = fname;
  
  toa_text -> clear();

  char useful[256];

  for (unsigned i = 0; i < toas.size(); i++) {
    toas[i].unload(useful);
    toa_text -> insertItem(useful);

    if (toas[i].get_format() == Tempo::toa::Command) {
      toas[i].ci = 4;
      toas[i].di = 4;
    }
  }
  
  toas_modified = false;

  if (autofit)
    fit ();
}

void Rhythm::add_toas (const char* fname)
{
  if (verbose)
    cerr << "Adding in TOAs from '" << fname << "'";
  
  vector<Tempo::toa> new_toas;
  
  Tempo::toa::load (fname, &new_toas);
  
  if (verbose)
    cout << "...loaded " << new_toas.size() << " new TOA's" << endl;
  
  for (unsigned i = 0; i < new_toas.size(); i++)
    toas.push_back(new_toas[i]);

  char num[8];
  sprintf(num, "%d", toas.size());
  
  QString str = "Added ";
  str += num;
  str += " TOA's";
  
  footer->setText(str);
  
  if (verbose)
    cout << " done." << endl;
  
  tempo->setItemEnabled (fitID, true);
  tempo->setItemEnabled (fitSelID, true);

  toa_filename = "newfile.tim";
  
  toa_text -> clear();

  char useful[256];

  for (unsigned i = 0; i < toas.size(); i++) {
    toas[i].unload(useful);
    toa_text -> insertItem(useful);

    if (toas[i].get_format() == Tempo::toa::Command) {
      toas[i].ci = 4;
      toas[i].di = 4;
    }
  }
  
  toas_modified = true;

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

  QString str = "TOA list saved";
  
  footer->setText(str);
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

  tempo->setItemEnabled (fitID, false);
  tempo->setItemEnabled (fitSelID, false);

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
    QMessageBox::critical (this, "Rhythm Show Profile",
			   "Error processing archive on disk.",
			   "Sigh.");
  }
  
}

void Rhythm::fit()
{
  if (!fitpopup || !fitpopup -> hasdata())
    return;
  
  if (toas.size() < 1) {
    if (verbose)
      cerr << "Rhythm::fit No Arrival Times loaded" << endl;
    return;
  }

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
{ 
  try {
    
    if (toas.size() < 1) {
      if (verbose)
	cerr << "Rhythm::fit No Arrival Times loaded" << endl;
      return;
    }
    
    if (verbose)
      cerr << "Rhythm::fit Calculating residuals" << endl;
    
    psrephem pf_eph;
    
    Tempo::fit (eph, toas, &pf_eph, false);
    
    if (load_new && fitpopup) {
      // set_psrephem will result in generation of newEph signal, 
      // which should be ignored since it was set from here.
      ignore_one_eph = true;
      
      if (verbose)
	cerr << "Rhythm::fit Displaying new ephemeris" << endl;
      
      fitpopup -> set_psrephem (pf_eph);
    }
    
    char temp[128];
    
    FILE* fptr = popen("tail -1 tempo.lis", "r");
    fgets(temp, 1024, fptr);
    pclose(fptr);
    
    string temp2 = temp;
    string temp3 = "Residual: " + temp2.substr(23, 54);

    footer->setText(temp3.c_str());
    
  } 
  catch (Error& error) {
    if (verbose)
      cerr << "Rhythm::fit ERROR " << error << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured.",
			   "Acknowledge");
  }
  catch (...) {
    if (verbose)
      cerr << "Rhythm::fit ERROR Unhandled Exception" << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured.",
			   "Acknowledge");
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
  plot_window->autoscale();

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
    
    unsigned tally = 0;
    
    for (unsigned i = 0; i < toas.size(); i++) {
      if (toas[i].get_state() == Tempo::toa::Selected)
	tally++;
    }
    
    if (tally < 2) {
      if (verbose)
	cerr << "Rhythm::fit_selected Not Enough Arrival Times selected" << endl;
      return;
    }
    
    if (verbose)
      cerr << "Rhythm::fit_selected Calculating residuals" << endl;
    
    psrephem pf_eph;
    
    Tempo::fit (eph, toas, &pf_eph, false, Tempo::toa::Selected);
    
    if (load_new && fitpopup) {
      // set_psrephem will result in generation of newEph signal, 
      // which should be ignored since it was set from here.
      ignore_one_eph = true;
      
      if (verbose)
	cerr << "Rhythm::fit_selected Displaying new ephemeris" << endl;
      
      fitpopup -> set_psrephem (pf_eph);
    }
    
    char temp[128];
    
    FILE* fptr = popen("tail -1 tempo.lis", "r");
    fgets(temp, 1024, fptr);
    pclose(fptr);
    
    string temp2 = temp;
    string temp3 = "Residual: " + temp2.substr(23, 54);

    footer->setText(temp3.c_str());

  } 
  catch (Error& error) {
    if (verbose)
      cerr << "Rhythm::fit_selected ERROR " << error << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured.  ",
			   "Acknowledge...");
  }
  catch (...) {
    if (verbose)
      cerr << "Rhythm::fit_selected ERROR Unhandled Exception" << endl;
    QMessageBox::critical (this, "Rhythm TOA Fitting Error",
			   "An unexpected exception occured",
			   "Acknowledge");
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
  
  char useful[80];
  char filename[80];
  
  psrephem eph;
  fitpopup -> get_psrephem (eph);
  
  QProgressDialog progress( "Calculating Data...", "Abort", toas.size(),
			    this, "progress", TRUE );
  
  switch (q) {

  case toaPlot::TOA_MJD:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back((toas[i].resid.mjd)-50000.0);
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::BinaryPhase:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.binaryphase);
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ObsFreq:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.obsfreq);
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::DayOfYear:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(fmod(toas[i].resid.mjd, 365.0));
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ResidualMicro:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.time);
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ParallacticAngle:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      // Extract the time
      MJD mjd(toas[i].resid.mjd);
      
      // Extract the coordinates
      sky_coord crd(eph.value_str[EPH_RAJ][0], eph.value_str[EPH_DECJ][0]);
      
      // Extract the lat and lon
      char telid = eph.value_str[EPH_TZRSITE][0];
      
      double lat = Pulsar::tzr_lat(telid);
      double lon = Pulsar::tzr_lon(telid);
      
      retval.push_back(Pulsar::parallactic_angle(crd, mjd, lat, lon));
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ResidualMilliTurns:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back((toas[i].resid.turns)*1000.0);
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::ErrorMicro:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      retval.push_back(toas[i].resid.error);
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::SignalToNoise:

    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();
      
      if ( progress.wasCancelled() )
	break;
      
      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_StoN() >= 0.0) {
	retval.push_back(toas[i].get_StoN());
	continue;
      }
      
      toas[i].unload(useful);
      
      if (sscanf(useful+1, "%s ", filename) != 1) {
	throw Error(FailedCall, "No archive-derived info available");
      }
      else {
	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	
	data->fscrunch();
	data->tscrunch();
	data->pscrunch();
	
	toas[i].set_StoN(data->get_Profile(0,0,0)->snr()); 
	retval.push_back(toas[i].get_StoN());
      }
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::Bandwidth:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();
      
      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_bw() != 0.0) {
	retval.push_back(toas[i].get_bw());
	continue;
      }
      
      toas[i].unload(useful);
      
      if (sscanf(useful+1, "%s ", filename) != 1) {
	throw Error(FailedCall, "No archive-derived info available");
      }
      else {
	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	
	toas[i].set_bw(data->get_bandwidth());
	retval.push_back(toas[i].get_bw());      
      }
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;
    
  case toaPlot::DispersionMeasure:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();

      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_dm() > 0.0) {
	retval.push_back(toas[i].get_dm());
	continue;
      }
      
      toas[i].unload(useful);
      
      if (sscanf(useful+1, "%s ", filename) != 1) {
	throw Error(FailedCall, "No archive-derived info available");
      }
      else {
	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	
	toas[i].set_dm(data->get_dispersion_measure());
	retval.push_back(toas[i].get_dm());
      }
    }
    progress.setProgress( toas.size() );
    
    return retval;
    break;

  case toaPlot::Duration:
    for ( unsigned i = 0; i < toas.size(); i++ ) {
      progress.setProgress( i );
      myapp->processEvents();
      
      if (toas[i].get_format() == Tempo::toa::Command) {
	retval.push_back(0.0);
	continue;
      }
      
      if (toas[i].get_dur() > 0.0) {
	retval.push_back(toas[i].get_dur());
	continue;
      }
      
      toas[i].unload(useful);
      
      if (sscanf(useful+1, "%s ", filename) != 1) {
	throw Error(FailedCall, "No archive-derived info available");
      }
      else {
	if (verbose)
	  cerr << "Attempting to load archive '" << filename << "'" << endl;
	
	string useful2 = dataPath + "/";
	useful2 += filename;
	
	Reference::To<Pulsar::Archive> data = Pulsar::Archive::load(useful2);
	
	toas[i].set_dur(data->integration_length());
	retval.push_back(toas[i].get_dur());     
      }
    }
    progress.setProgress( toas.size() );
    
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
  
  // This may require adjustment! Find out what the error
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
    
  case toaPlot::ParallacticAngle:
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

  case toaPlot::Bandwidth:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::DispersionMeasure:
    for (unsigned i = 0; i < toas.size(); i++)
      retval.push_back(0.0);
    return retval;
    break;

  case toaPlot::Duration:
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
  vector<double> xerrs = give_me_errs(xq);
  vector<double> yerrs = give_me_errs(yq);
  
  vector<wrapper> useme;
  
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_state() != Tempo::toa::Deleted) {
      
      wrapper tempw;
      
      if (toas[i].get_format() == Tempo::toa::Command) {
	
	if (i == 0) {
	  tempw.x = tempx[1];
	  tempw.y = tempy[1];
	  tempw.ex = 0.0;
	  tempw.ey = 0.0;
	  
	  tempw.dot = toas[i].di;
	  tempw.id = 0;
	  
	  if (toas[i].get_state() == Tempo::toa::Selected)
	    tempw.ci = 2;
	  else
	    tempw.ci = toas[i].ci;

	  useme.push_back(tempw);
	}
	else if (i == toas.size() - 1) {
	  
	  int ind = toas.size() - 2;
	  
	  tempw.x = tempx[ind];
	  tempw.y = tempy[ind];
	  tempw.ex = 0.0;
	  tempw.ey = 0.0;
	  
	  tempw.dot = toas[i].di;
	  tempw.id = toas.size() - 1;
	  
	  if (toas[i].get_state() == Tempo::toa::Selected)
	    tempw.ci = 2;
	  else
	    tempw.ci = toas[i].ci;

	  useme.push_back(tempw);
	}
	else {
	  
	  tempw.x = (tempx[i-1] + tempx[i+1]) / 2.0;
	  tempw.y = (tempy[i-1] + tempy[i+1]) / 2.0;
	  tempw.ex = 0.0;
	  tempw.ey = 0.0;

	  tempw.id = i;

	  if (toas[i].get_state() == Tempo::toa::Selected)
	    tempw.ci = 2;
	  else
	    tempw.ci = toas[i].ci;
	  
	  tempw.dot = toas[i].di;
	  
	  useme.push_back(tempw);
	}
      }
      else {

	tempw.x = tempx[i];
	tempw.y = tempy[i];
	tempw.ex = xerrs[i];
	tempw.ey = yerrs[i];
	tempw.id = i;
	
	tempw.dot = toas[i].di;
	
	if (toas[i].get_state() == Tempo::toa::Selected)
	  tempw.ci = 2;
	else
	  tempw.ci = toas[i].ci;
	
	useme.push_back(tempw);
      }
    }
  }
  
  plot_window->setPoints(xq, yq, useme);
}

void Rhythm::reselect ()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_state() == Tempo::toa::Deleted)
      continue;
    
    if (toa_text->isSelected(i))
      toas[i].set_state(Tempo::toa::Selected);
    else
      toas[i].set_state(Tempo::toa::Normal);
  }
}

void Rhythm::deselect (int pt)
{
  if (pt >= int(toas.size()))
    return;
  if (pt < 0)
    return;
  
  if (toas[pt].get_state() == Tempo::toa::Deleted)
    return;
  
  toas[pt].set_state(Tempo::toa::Normal);
  toa_text -> setSelected (pt, false);
  
}

void Rhythm::deselect (vector<int> pts)
{
  QProgressDialog progress( "De-Selecting Points...", "Abort", pts.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < pts.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    deselect(pts[i]);
  }
  progress.setProgress( pts.size() );
}

void Rhythm::request_update ()
{
  goplot ();
}

void Rhythm::select (int pt)
{
  int size = toas.size();
  
  if (pt >= size)
    return;
  if (pt < 0)
    return;
  
  if (toas[pt].get_state() == Tempo::toa::Deleted)
    return;
  
  toas[pt].set_state(Tempo::toa::Selected);
  toa_text -> setSelected (pt, true);
  toa_text -> setCurrentItem (pt);
}

void Rhythm::select (vector<int> pts)
{
  QProgressDialog progress( "Selecting Points...", "Abort", pts.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < pts.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    select(pts[i]);
  }
  progress.setProgress( pts.size() );
  
}

void Rhythm::freqsort ()
{
  for (unsigned i = 0; i < toas.size(); i++) {
    if (toas[i].resid.obsfreq != 0.0) {
      if (toas[i].resid.obsfreq < 500.0)
	toas[i].ci = 3;
      else if (toas[i].resid.obsfreq < 800.0)
	toas[i].ci = 4;
      else if (toas[i].resid.obsfreq < 1600.0)
	toas[i].ci = 5;
      else if (toas[i].resid.obsfreq < 2400.0)
	toas[i].ci = 6;
      else if (toas[i].resid.obsfreq < 5500.0)
	toas[i].ci = 8;
      else if (toas[i].resid.obsfreq < 9000.0)
	toas[i].ci = 9;
      else
	toas[i].ci = 10;
    }
    else if (toas[i].get_frequency() != 0.0) {
      if (toas[i].get_frequency() < 500.0)
	toas[i].ci = 3;
      else if (toas[i].get_frequency() < 800.0)
	toas[i].ci = 4;
      else if (toas[i].get_frequency() < 1600.0)
	toas[i].ci = 5;
      else if (toas[i].get_frequency() < 2400.0)
	toas[i].ci = 6;
      else if (toas[i].get_frequency() < 5500.0)
	toas[i].ci = 8;
      else if (toas[i].get_frequency() < 9000.0)
	toas[i].ci = 9;
      else
	toas[i].ci = 10;
    }
    else {
      footer->setText("No frequency information available!");
      return;
    }
  }
}

void Rhythm::change_mode (int m)
{
  mode = m;
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
  QProgressDialog progress( "Deleting Points...", "Abort", toas.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < toas.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    if (toas[i].get_state() == Tempo::toa::Selected)
      toas[i].set_state(Tempo::toa::Deleted);
  }
  progress.setProgress( toas.size() );
  
  toas_modified = true;
  
  goplot ();
  plot_window->autoscale();
}

void Rhythm::undeleteall ()
{
  QProgressDialog progress( "Restoring Deleting Points...", "Abort", toas.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < toas.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    if (toas[i].get_state() == Tempo::toa::Deleted) {
      toas[i].set_state(Tempo::toa::Normal);
      toa_text -> setSelected (i, false);
    }
  }
  progress.setProgress( toas.size() );
  
  goplot ();
  plot_window->autoscale();
}


void Rhythm::clearselection ()
{
  QProgressDialog progress( "Clearing Selection List...", "Abort", toas.size(),
			    this, "progress", TRUE );
  for ( unsigned i = 0; i < toas.size(); i++ ) {
    progress.setProgress( i );
    myapp->processEvents();
    
    if ( progress.wasCancelled() )
      break;
    
    if (toas[i].get_state() == Tempo::toa::Deleted)
      continue;
    
    toas[i].set_state(Tempo::toa::Normal);
    toa_text -> setSelected (i, false);
  }
  progress.setProgress( toas.size() );
  
  goplot ();
}

void Rhythm::colour_selector ()
{
  int temp = -1;
  
  temp = QInputDialog::getInteger("Rhythm",
				  "Please enter the colour index: (0 -> 15)");
  
  if (temp >= 0 && temp < 16)
    setselcol (temp);
}

void Rhythm::setselcol (int index)
{
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_state() == Tempo::toa::Selected)
      toas[i].ci = index;
    
  }
  goplot ();
}

void Rhythm::symbol_selector ()
{
  int temp = -1;
  
  temp = QInputDialog::getInteger("Rhythm",
				  "Please enter the symbol index: (0 -> 127)");
  
  if (temp >= 0 && temp < 128)
    setseldot (temp);
}

void Rhythm::setseldot (int index)
{
  for (unsigned i = 0; i < toas.size(); i++) {
    
    if (toas[i].get_state() == Tempo::toa::Selected)
      toas[i].di = index;
    
  }
  goplot ();
}

AxisSelector::AxisSelector (QWidget* parent)
  : QHBox(parent)
{
  Xgrp = new QButtonGroup(12, Qt::Vertical, "X Axis", this);
  Xgrp -> setRadioButtonExclusive(true);
  
  Ygrp = new QButtonGroup(12, Qt::Vertical, "Y Axis", this);
  Ygrp -> setRadioButtonExclusive(true);

  X1 = new QRadioButton("Residual (us)", Xgrp);
  X2 = new QRadioButton("Residual (mt)", Xgrp);
  X3 = new QRadioButton("TOA (MJD)", Xgrp);
  X4 = new QRadioButton("Binary Phase", Xgrp);
  X5 = new QRadioButton("Obs Freq", Xgrp);
  X6 = new QRadioButton("Day of Year", Xgrp);
  X7 = new QRadioButton("Timing Error", Xgrp);
  X8 = new QRadioButton("Signal / Noise", Xgrp);
  X9 = new QRadioButton("Bandwidth", Xgrp);
  X10 = new QRadioButton("DM", Xgrp);
  X11 = new QRadioButton("Length", Xgrp);
  X12 = new QRadioButton("P.A.", Xgrp);

  X3->setChecked(true);

  Y1 = new QRadioButton("Residual (us)", Ygrp);
  Y2 = new QRadioButton("Residual (mt)", Ygrp);
  Y3 = new QRadioButton("TOA (MJD)", Ygrp);
  Y4 = new QRadioButton("Binary Phase", Ygrp);
  Y5 = new QRadioButton("Obs Freq", Ygrp);
  Y6 = new QRadioButton("Day of Year", Ygrp);
  Y7 = new QRadioButton("Timing Error", Ygrp);
  Y8 = new QRadioButton("Signal / Noise", Ygrp);
  Y9 = new QRadioButton("Bandwidth", Ygrp);
  Y10 = new QRadioButton("DM", Ygrp);
  Y11 = new QRadioButton("Length", Ygrp);
  Y12 = new QRadioButton("P.A.", Ygrp);
  
  Y1->setChecked(true);
  
  Xgrp->insert(X1,1);
  Xgrp->insert(X2,2);
  Xgrp->insert(X3,3);
  Xgrp->insert(X4,4);
  Xgrp->insert(X5,5);
  Xgrp->insert(X6,6);
  Xgrp->insert(X7,7);
  Xgrp->insert(X8,8);
  Xgrp->insert(X9,9);
  Xgrp->insert(X10,10);
  Xgrp->insert(X11,11);
  Xgrp->insert(X12,12);

  Ygrp->insert(Y1,1);
  Ygrp->insert(Y2,2);
  Ygrp->insert(Y3,3);
  Ygrp->insert(Y4,4);
  Ygrp->insert(Y5,5);
  Ygrp->insert(Y6,6);
  Ygrp->insert(Y7,7);
  Ygrp->insert(Y8,8);
  Ygrp->insert(Y9,9);
  Ygrp->insert(Y10,10);
  Ygrp->insert(Y11,11);
  Ygrp->insert(Y12,12);

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
