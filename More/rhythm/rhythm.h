#ifndef __RHYTHM_H
#define __RHYTHM_H

#include <qapplication.h>
#include <qvbox.h>
#include <qhbox.h>
#include <qmenubar.h>

#include <vector>

#include "toa.h"
#include "residual.h"
#include "psrephem.h"

#define TIM 1
#define EPH 2

class qt_psrephem_editor;

class Rhythm : public QWidget
{
protected:

  // an array of ephemerides and the filename of the last one loaded
  string eph_filename;
  vector<psrephem> ephemerides;

  // an array of toas and the filename from which they were loaded
  string tim_filename;
  vector<toa> arrival_times;

  // an array of residuals corresponding to the above toas.  These are plotted
  vector<residual> residuals;

  // the GUI bits
  QVBox    main_window;
  QMenuBar menu;
  QHBox    plots;

  // /////////////////////////////////////////////////////////////////////////
  // main menubar and its constructor
  void menubarConstruct ();    // construct the menubar along the top

  bool eph_modified;
  bool toas_modified;

  // /////////////////////////////////////////////////////////////////////////
  // Fit parameters menu widgets/routines
  qt_psrephem_editor* fitpopup;
  void fit_popup (int nothing);

  // /////////////////////////////////////////////////////////////////////////
  // Command line parsing and exit routines
  void exit (int value);
  void command_line_parse (int argc, char** argv);

public slots:
  void load_toas ();
  void prompt_save_toas ();
  void save_toas ();
  void load_eph ();
  void prompt_save_eph ();
  void save_eph ();
  void fit();
  void undo() {fprintf (stderr, "Not implemented\n");};
  void redo() {fprintf (stderr, "Not implemented\n");};

  void about();
  void aboutQt();

public:
  static int verbose;

  Rhythm (int argc, char** argv);
  ~Rhythm () {};

};

#endif
