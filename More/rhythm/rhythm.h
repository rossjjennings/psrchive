#ifndef __RHYTHM_H
#define __RHYTHM_H

#include <vector>
#include <gtk--.h>

#include "toa.h"
#include "residual.h"
#include "psrephem.h"

#define TIM 1
#define EPH 2

class rhythm : public Gtk_Window
{
protected:
  vector<psrephem> ephemerides;
  vector<toa>      arrival_times;
  vector<residual> residuals;

  Gtk_VBox  main_window;
  Gtk_HBox  plots;

  // /////////////////////////////////////////////////////////////////////////
  // main menubar and its constructor
  Gtk_ObjectHandle<Gtk_MenuBar> menubar;
  void menubarConstruct ();    // construct the menubar along the top

  // /////////////////////////////////////////////////////////////////////////
  // file loading/unloading menu widgets/routines
  Gtk_MenuItem* save_eph;
  Gtk_MenuItem* save_tim;
  Gtk_FileSelection* fileselect;

  int fileio_code;
  bool file_modified;

  string eph_filename;
  string tim_filename;

  void prompt_save (int type);
  void fileload (int type);    // controls the file selection popup 
  void filesave (int type);    // controls the file selection popup 
  gint fileselect_deleted (GdkEventAny*);   // called when the popup is deleted
  void filecancel();           // callback on cancel
  void fileselected();         // callback on selection

  // /////////////////////////////////////////////////////////////////////////
  // Fit parameters menu widgets/routines
  Gtk_Window* fitpopup;

  void fit_popup (int nothing);

  // /////////////////////////////////////////////////////////////////////////
  // Command line parsing and exit routines
  void exit (int value);
  void command_line_parse (int argc, char** argv);

public:
  static int verbose;

  rhythm (int argc, char** argv);
  ~rhythm () {};

};

#endif
