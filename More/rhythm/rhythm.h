//-*-C++-*-

/* $Source: /cvsroot/psrchive/psrchive/More/rhythm/rhythm.h,v $
   $Revision: 1.11 $
   $Date: 2000/05/31 15:06:34 $
   $Author: straten $ */

// //////////////////////////////////////////////////////////////////////////
//
// Rhythm
//
// A class for interactively plotting Tempo::ModelDataSet using the
// PlotManager and DataManager classes defined in genutil++
//
// //////////////////////////////////////////////////////////////////////////

#ifndef __RHYTHM_H
#define __RHYTHM_H

#include <vector>
#include <qmainwindow.h>

#include "ModelDataSet.h"
#include "Options.h"

class qt_editParams;
class DataManager;

class Rhythm : public QMainWindow
{
  Q_OBJECT

 public:
  static bool verbose;
  static bool vverbose;
  
  Rhythm (QWidget* parent, int argc, char** argv);
  ~Rhythm () {};

  // fits the loaded toas using TEMPO with the given psrParams, 'eph'.
  // if 'load_new' is true, loads the new epehemeris into the display
  void fit (const psrParams& eph, bool load_new);

  // set the verbosity flag for a number of the classes on which
  // this application is based
  void setClassVerbose (bool verbose);

 protected:
  RhythmOptions opts;

  // an array of toas and the filename from which they were loaded
  Tempo::ModelDataSet modelPlot;
  Tempo::Model model;

  void load_toas (const char* fname);

  // /////////////////////////////////////////////////////////////////////////
  // Main Plotting Window(s)
  vector<QWidget*> plot_manager;   // vector of plot manager widgets
  vector<string> plot_descriptor;  // vector of strings for each manager
  vector<int> plot_id;             // Options menu id for each manager

  vector<DataManager*> data_manager;

  int plot_selected_id;
  void initializePlot ();            // defined in initializePlot.C

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
  QPopupMenu *tempo;      // pointer to the Tempo popup menu
  int dispID;             // ID of the 'Display Parameters' menu item
  int fitID;              // ID of the 'Fit' menu item
  int autofitID;          // ID of the 'Autofit' menu item
  int saveParmsID;

  bool autofit;           // fit whenever TOAs or TEMPO Parameters are loaded
  bool ignore_one_eph;    // ignore the newParams signal from qt_editeph ONCE
  bool toas_modified;     // the toas have been modified since loaded

  // The OPTIONS popup menu and IDs of its individual options
  QPopupMenu *options;
  int quietID;
  int mediumID;
  int noisyID;

  // /////////////////////////////////////////////////////////////////////////
  // Command line parsing and exit routines
  void exit (int value);
  void command_line_help ();
  void command_line_parse (int argc, char** argv);

 public slots:
  // File menu callbacks
  void load_toas ();
  void prompt_save_toas ();
  void save_toas ();

  // Tempo menu callbacks
  void fit();
  void togledit();
  void toglauto();

  // Options->Verbosity menu callback
  void setVerbosity (int id);
  // Options->Plotter menu callback
  void setPlotter ( int plotterID );
  // Options->Preferences
  void showOptions () { opts.show(); };

  void undo() {fprintf (stderr, "Not implemented\n");};
  void redo() {fprintf (stderr, "Not implemented\n");};

  // Help menu callbacks
  void about();
  void aboutQt();
  void closeWin() {fprintf (stderr, "Not implemented\n");};

  // callback connected to qt_editParams::newParams
  void set_Params (const psrParams&);
};

#endif
