/* ///////////////////////////////////////////////////////////////////////
   The Qt editeph Widget
      Manipulate Qt psrParams widget with this gui component

   Author: Willem van Straten 
   /////////////////////////////////////////////////////////////////////// */

#ifndef __QT_EDITEPH_H
#define __QT_EDITEPH_H

#include <vector>

#include "qt_psrParams.h"

class qt_fileParams;

class qt_editParams : public QWidget
{
  Q_OBJECT

 public:
  static bool verbose;
  static int  border_width;  // size of border around widget frame

  qt_editParams (QWidget* parent=0, const char* name=0);

  // load/unload ephemeris from/to filename
  void load   (const char* filename);
  void unload (const char* filename);

  // display the ephemeris given / get the epehemeris displayed
  void set_psrParams (const psrParams& eph);
  void get_psrParams (psrParams* eph);

  // returns true if the editor currently contains a set of TEMPO 11 parameters
  bool hasdata() { return current != -1; };

 signals:
  // emitted when the user selects 'File/Close'
  void closed ();
  void newParams ( const psrParams& );

 public slots:
  // File Menu Callbacks
  void open();
  void save();
  void print();
  void closeWin();

  // Edit Menu Callbacks
  void forward();          // go forward to newer parameter set
  void backward();         // go back to last parameter set
  void select_parms();     // hides/shows the selection grid

  // Help Menu Callbacks
  void about();
  void aboutQt();

 private:

  // the psr parameters display widget
  qt_psrParams display;

  // the parameters kept in the editor display
  psrParams data;

  // history of parameter sets as the user loads and modifies
  vector<psrParams> data_history;
  int current;

  void menubarConstruct ();   // constructs the menubar
  QWidget* menu;              // the menubar
  
  void parmboxConstruct ();   // constructs a dialog box with qt_toglParams
  QWidget* eph_select;        // the dialog created in parmboxConstruct ();

  // the open/save popup dialog
  qt_fileParams* io_dialog;

  // performs various book-keeping tasks whenever
  // a new TEMPO11 style psrParams is loaded
  void new_data (bool add_to_history = true);

  // ///////////////////////////////////////////////////////////////////
  // the popup menus
  QPopupMenu* file;
  int saveID;

  QPopupMenu* edit;   // Pointer to the edit popup menu
  int selectID;       // ID of parameter select menu option
  int backwardID;     // ID of backward menu option
  int forwardID;      // ID of forward menu option

 protected:
  // Functions that assist the geometry manager
  QSize sizeHint () const;
  QSizePolicy sizePolicy () const 
    { return QSizePolicy (QSizePolicy::Minimum, QSizePolicy::Expanding); };
  QSize minimumSizeHint () const 
    { return sizeHint(); };
};

#endif
