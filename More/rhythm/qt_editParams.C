
/* PLANS:
   1) qt_psrParams not a data member.  Instead, this object acts on its own
   and has a method that points it at a qt_psrParams object (or something
   like that)

   2) to protect against segfaults.  qt_psrParams will emit a signal when
   destroyed.

   */

#include <iostream>
#include <vector>

#include <qapplication.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qpushbutton.h>
#include <qmenubar.h>

#include "qt_editParams.h"
#include "qt_toglParams.h"
#include "qt_fileParams.h"
#include "qtools.h"
#include "psr_cpp.h"

// //////////////////////////////////////////////////////////////////////////
//
// qt_editParams
//
// A box with checkbuttons for each type of ephemeris value
//
// //////////////////////////////////////////////////////////////////////////

bool qt_editParams::verbose = false;

qt_editParams::qt_editParams (QWidget* parent, const char* name)
  : QWidget  (parent, name),
    display (this, "ephemeris")
{
  menubarConstruct ();

  QBoxLayout* layout = new QVBoxLayout (this, border_width);

  layout -> setMenuBar ( (QMenuBar*) menu );
  layout -> addWidget ( &display );
  layout -> setResizeMode ( QLayout::Minimum );
  layout -> activate ();

  parmboxConstruct ();

  io_dialog = NULL;
  current = -1;
}

// //////////////////////////////////////////////////////////////////////////
//
// qt_editParams::menubarConstruct ()
//
// Constructs the menubar at the top of the editor
//
// //////////////////////////////////////////////////////////////////////////

void qt_editParams::menubarConstruct ()
{
  menu = new QMenuBar (this, "menubar");

  // the File Menu
  file = new QPopupMenu( menu );
  CHECK_PTR( file );
  file->insertItem( "&Open", this, SLOT(open()), ALT+Key_O );
  saveID = file->insertItem( "&Save As",
			     this, SLOT(save()), ALT+Key_S );
  file->setItemEnabled (saveID, false);

  file->insertSeparator();
  file->insertItem( "&Print (stderr)",
		    this, SLOT( print() ), ALT+Key_P );

  if (isTopLevel()) {
    file->insertSeparator();
    file->insertItem( "&Close", this, SLOT(closeWin()), ALT+Key_C );
    file->insertItem( "E&xit",  qApp, SLOT(quit()), ALT+Key_X );
  }

  // the Edit Menu
  edit = new QPopupMenu( menu );
  CHECK_PTR( edit );

  backwardID = edit->insertItem ("&Backward",
				 this, SLOT(backward()), ALT+Key_B );
  edit->setItemEnabled (backwardID, false);

  forwardID = edit->insertItem ("&Forward",
				this, SLOT(forward()), ALT+Key_F );
  edit->setItemEnabled (forwardID, false);

  edit->insertSeparator();
  selectID = edit->insertItem ("Select P&arameters", 
				  this, SLOT(select_parms()), ALT+Key_A );

  // the Help Menu
  QPopupMenu *help = new QPopupMenu( menu );
  CHECK_PTR( help );
  help->insertItem( "Usage", this, SLOT(about()), ALT+Key_U );
  if (isTopLevel())
    help->insertItem( "About Qt", this, SLOT(aboutQt()), 0);

  //
  // the whole thing
  //
  ((QMenuBar*) menu) -> insertItem   ( "&File", file );
  ((QMenuBar*) menu) -> insertItem   ( "&Edit", edit );
  ((QMenuBar*) menu) -> insertSeparator();
  ((QMenuBar*) menu) -> insertItem   ( "&Help", help );
  ((QMenuBar*) menu) -> setSeparator ( QMenuBar::InWindowsStyle );

  return;
}

// //////////////////////////////////////////////////////////////////////////
//
// qt_editParams::parmboxConstruct ()
//
// Constructs the popup "dialog" with all of the available TEMPO parameters
//
// //////////////////////////////////////////////////////////////////////////
int qt_editParams::border_width = 4;

void qt_editParams::parmboxConstruct ()
{
  // make a new top-level, and not a dialog
  eph_select = new QVBox (0, "eph selection");
  ((QVBox*) eph_select) -> setMargin (4);

  qt_toglParams* parm = new qt_toglParams (eph_select, "table");

  eph_select -> setMaximumWidth ( parm -> sizeHint().width() + 8 );

  if (verbose)
    parm -> installEventFilter ( new qevent_trace );

  connect (parm, SIGNAL ( getChecked(int,bool) ), 
	   &display, SLOT ( show_item(int,bool) ));
  
  connect (&display, SIGNAL ( item_shown(int,bool) ),
	   parm, SLOT ( setChecked(int,bool) ));

  QButton* dismiss = new QPushButton ("Dismiss", eph_select, "dismiss");
  dismiss -> setMaximumSize ( dismiss -> sizeHint() );

  connect (dismiss, SIGNAL(clicked()), this, SLOT(select_parms()));
}

// //////////////////////////////////////////////////////////////////////////
//
// qt_editParams::select_parms()
//
// Toggles the above popup "dialog" in and out of sight
//
// //////////////////////////////////////////////////////////////////////////

void qt_editParams::select_parms()
{
  if (!eph_select)
    return;

  if (eph_select->isVisible()) {
    eph_select->hide();
    edit->setItemEnabled (selectID, TRUE);
  }
  else {
    eph_select->show();
    edit->setItemEnabled (selectID, FALSE);
  }
}

// //////////////////////////////////////////////////////////////////////////
//
// qt_editParams::new_data ()
//
// Called any time a new ephemeris is loaded from file, set from the program,
// etc.  Performs various house-keeping tasks.
//
// //////////////////////////////////////////////////////////////////////////

void qt_editParams::new_data (bool add_to_history)
{
  if (verbose)
    cerr << "qt_editParams::new_data set display" << endl;
  display.set_psrParams (data);

  if (add_to_history) {
    if (verbose)
      cerr << "qt_editParams::new_data add to history" << endl;
    current ++;
    data_history.resize (current + 1);
    data_history [current] = data;
  }

  if (verbose)
    cerr << "qt_editParams::new_data adjust menus" << endl;
  edit->setItemEnabled (backwardID, (current > 0));
  edit->setItemEnabled (forwardID, (current < int(data_history.size()) - 1));
  file->setItemEnabled (saveID, true);

  if (verbose)
    cerr << "qt_editParams::new_data emit newParams" << endl;
  emit newParams ( data );
}

void qt_editParams::open() 
{
  if (!io_dialog)
    io_dialog = new qt_fileParams;

  if (io_dialog -> open (&data))
    new_data ();
}

void qt_editParams::save() 
{
  if (!hasdata())
    return;
  display.get_psrParams (&data);

  if (!io_dialog)
    io_dialog = new qt_fileParams;
  io_dialog -> save (data);
}

void qt_editParams::print()
{ 
  display.get_psrParams().unload (stderr);
}

void qt_editParams::closeWin()
{
  emit closed();
  hide();
}

void qt_editParams::load (const char* filename)
{
  if (verbose)
    cerr << "qt_editParams::load psrParameters::load(" << filename << ")...";
  data.load (filename);
  if (verbose)
    cerr << "  loaded." << endl;
  new_data ();
}

void qt_editParams::unload (const char* filename)
{
  if (!hasdata())
    return;

  display.get_psrParams (&data);
  data.unload (filename);
}

void qt_editParams::set_psrParams (const psrParams& eph)
{
  data = eph;
  new_data ();
}

void qt_editParams::get_psrParams (psrParams* eph)
{
  display.get_psrParams (eph);
}

void qt_editParams::forward()
{
  if (current >= int( data_history.size() ))
    return;

  current ++;
  data = data_history [current];
  new_data (false);
}

void qt_editParams::backward()
{
  if (current < 1)
    return;

  current --;
  data = data_history [current];
  new_data (false);
}

void qt_editParams::about()
{
  QMessageBox::about (NULL, "PSR Ephemeris Editor",
		      "Use the File Menu to Open/Save a TEMPO ephemeris.\n"
		      "Use the Edit Menu to Add/Remove parameters." );
}

void qt_editParams::aboutQt()
{
  QMessageBox::aboutQt (NULL, "Qt Information");
}

QSize qt_editParams::sizeHint () const 
{ 
  if (verbose)
    cerr << "qt_editParams::sizeHint = " << display.sizeHint() << endl;

  return display.sizeHint();
}
