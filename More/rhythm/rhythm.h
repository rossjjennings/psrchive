#ifndef __RHYTHM_H
#define __RHYTHM_H

#include <vector>

#include <qmainwindow.h>

#include "qxmp_manager.h"
#include "pg_2dplot.h"

#include "toa.h"
#include "residual.h"
#include "psrParams.h"

class qt_editParams;

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
 
 protected:
  // an array of toas and the filename from which they were loaded
  vector<toa> arrival_times;

  void load_toas (const char* fname);
  string tim_filename;

  // an array of residuals corresponding to the above toas.  These are plotted
  vector<residual> residuals;

  // the GUI bits
  qxmp_manager plot_manager;
  xyplot       res_plot;

  // /////////////////////////////////////////////////////////////////////////
  // main menubar and its constructor
  void menubarConstruct ();    // construct the menubar along the top

  // The TEMPO popup menu and IDs of its individual options
  QPopupMenu *tempo;      // pointer to the Tempo popup menu
  int dispID;             // ID of the 'Display Parameters' menu item
  int fitID;              // ID of the 'Fit' menu item
  int autofitID;          // ID of the 'Autofit' menu item

  bool autofit;           // fit whenever TOAs or TEMPO Parameters are loaded
  bool ignore_one_eph;    // ignore the newParams signal from qt_editeph ONCE
  bool toas_modified;     // the toas have been modified since loaded

  // /////////////////////////////////////////////////////////////////////////
  // Fit parameters menu widgets/routines
  qt_editParams* fitpopup;
  //void fit_popup (int nothing);

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
