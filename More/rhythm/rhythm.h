//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/rhythm/rhythm.h,v $
   $Revision: 1.32 $
   $Date: 2003/07/31 08:35:23 $
   $Author: ahotan $ */

// //////////////////////////////////////////////////////////////////////////
//
// Rhythm
//
// A Graphical User Interface to Tempo
//
// //////////////////////////////////////////////////////////////////////////

#ifndef __RHYTHM_H
#define __RHYTHM_H

#include <vector>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qlistbox.h>
#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qinputdialog.h>
#include "qprogressdialog.h"

#include "psrephem.h"
#include "ephio.h"
#include "toaPlot.h"
#include "toa.h"
#include "residual.h"

#include "sky_coord.h"
#include "MJD.h"

#include "Options.h"

#include "Pulsar/Plotter.h"
#include "Pulsar/Archive.h"
#include "Pulsar/Profile.h"
#include "Pulsar/Calculator.h"

class qt_editParams;

class AxisSelector : public QHBox
{
 Q_OBJECT
  
 public:

  AxisSelector (QWidget* parent);

 protected:

  QButtonGroup* Xgrp;
  QButtonGroup* Ygrp;

  QRadioButton* X1;
  QRadioButton* X2; 
  QRadioButton* X3;
  QRadioButton* X4;
  QRadioButton* X5; 
  QRadioButton* X6;
  QRadioButton* X7;
  QRadioButton* X8;
  QRadioButton* X9;
  QRadioButton* X10;
  QRadioButton* X11;
  QRadioButton* X12;

  QRadioButton* Y1;
  QRadioButton* Y2;
  QRadioButton* Y3;
  QRadioButton* Y4;
  QRadioButton* Y5;
  QRadioButton* Y6;
  QRadioButton* Y7;
  QRadioButton* Y8;
  QRadioButton* Y9;
  QRadioButton* Y10;
  QRadioButton* Y11;
  QRadioButton* Y12;
  
 public slots:

  void Xuseful(int);
  void Yuseful(int);

 signals:

  void XChange (toaPlot::AxisQuantity);
  void YChange (toaPlot::AxisQuantity);
};

class Rhythm : public QMainWindow
{
  Q_OBJECT

 public:
  static bool verbose;
  static bool vverbose;
  
  Rhythm (QApplication* master, QWidget* parent, int argc, char** argv);
  ~Rhythm () {};

  // fits the loaded toas using TEMPO with the given psrephem, 'eph'.
  // if 'load_new' is true, loads the new epehemeris into the display
  void fit (const psrephem& eph, bool load_new);

  // fits the selected toas using TEMPO with the given psrephem, 'eph'.
  // if 'load_new' is true, loads the new epehemeris into the display
  void fit_selected (const psrephem& eph, bool load_new);

  // set the verbosity flag for a number of the classes on which
  // this application is based
  void setClassVerbose (bool verbose);

 protected:

  QApplication* myapp;

  RhythmOptions opts;
  
  // an array of toas
  vector<Tempo::toa> toas;
  // the file from which they were read
  string toa_filename;
  
  // the central display slate
  QListBox* toa_text;

  void load_toas (const char* fname);
  void add_toas (const char* fname);
  void save_toas (const char* fname);

  void update_mode ();

  // /////////////////////////////////////////////////////////////////////////
  // Main Plotting Window(s)
  toaPlot* plot_window;
  AxisSelector* chooser;
  toaPlot::AxisQuantity xq;
  toaPlot::AxisQuantity yq;

  QVBox* leftpanel;
  QHBox* bottompanel;
  QVBox* controls;
  QLabel* header;
  QLabel* footer;
  QButtonGroup* modechanger;
  QRadioButton* zoom;
  QRadioButton* sel;

  QPushButton* cut;
  
  QPushButton* xrange;
  QPushButton* yrange;
  QPushButton* point;
  QPushButton* box;
  
  QPushButton* clearsel;
  QPushButton* undel;
  QPushButton* autoscl;
  QPushButton* freqs;
  QPushButton* show_button;

  QPushButton* colour;
  QPushButton* dotify;

  string dataPath;

  int mode;
  
  // /////////////////////////////////////////////////////////////////////////
  // Fit parameters Menu Widget
  qt_editParams* fitpopup;

  // /////////////////////////////////////////////////////////////////////////
  // ToolBar and its constructor
  void toolbarConstruct ();

  // /////////////////////////////////////////////////////////////////////////
  // main menubar and its constructor
  void menubarConstruct ();    // construct the menubar along the top

  // The TEMPO popup menu and IDs of its individual options
  QPopupMenu* tempo;      // pointer to the Tempo popup menu
  int dispID;             // ID of the 'Display Parameters' menu item
  int fitID;              // ID of the 'Fit' menu item
  int fitSelID;           // ID of the 'Fit Selected' menu item
  int autofitID;          // ID of the 'Autofit' menu item
  int temsysID;
  int ddkID;
  int stdID;
  int weightsID;          // ID of the 'Weights' menu item
  int saveParmsID;

  bool autofit;           // fit whenever TOAs or TEMPO Parameters are loaded
  bool weights;           // use TEMPO mode 1
  bool ignore_one_eph;    // ignore the newParams signal from qt_editeph ONCE
  bool toas_modified;     // the toas have been modified since loaded

  // The OPTIONS popup menu and IDs of its individual options
  QPopupMenu* options;
  int quietID;
  int mediumID;
  int noisyID;

  // /////////////////////////////////////////////////////////////////////////
  // Command line parsing and exit routines
  void exit (int value);
  void command_line_help ();
  void command_line_parse (int argc, char** argv);

  // /////////////////////////////////////////////////////////////////////////
  // Data extraction routines
  vector<double> give_me_data (toaPlot::AxisQuantity);
  vector<double> give_me_errs (toaPlot::AxisQuantity);

 public slots:

  void show_me ();
  void request_update ();

  // File menu callbacks
  void load_toas ();
  void add_toas ();
  void close_toas ();
  int  prompt_save_toas ();
  void save_toas ();

  // Tempo menu callbacks
  void fit();
  void fit_selected();
  void togledit();
  void toglauto();
  void toglweights();
  void temposys();
  void set_ddk();
  void set_std_tempo();

  // Options->Verbosity menu callback
  void setVerbosity (int id);
  // Options->Preferences
  void showOptions () { opts.show(); };

  void undo() {fprintf (stderr, "Not implemented\n");};
  void redo() {fprintf (stderr, "Not implemented\n");};

  // Help menu callbacks
  void about();
  void aboutQt();

  // callback connected to qt_editParams::newParams
  void set_Params (const psrephem&);

  // plot selection slots
  void XChange (toaPlot::AxisQuantity);
  void YChange (toaPlot::AxisQuantity);
  void goplot ();

  void change_mode (int);

  void xrange_slot ();
  void yrange_slot ();
  void point_slot ();
  void box_slot ();
  
  void select (int);
  void select (vector<int>);
  void deselect (int);
  void deselect (vector<int>);
  void reselect ();
  
  void clearselection ();
  void deleteselection ();

  void colour_selector ();
  void symbol_selector ();

  void setselcol (int);
  void setseldot (int);

  void undeleteall ();

  void freqsort ();
  
  void setDataPath ();
};

#endif








