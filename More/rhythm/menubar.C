#include <iostream>

#include <qapplication.h>
#include <qpopupmenu.h> 
#include <qmenubar.h> 
#include <qmessagebox.h>
#include <qfiledialog.h>

#include "rhythm.h"

void Rhythm::menubarConstruct ()
{
  // ///////////////////////////////////////////////////////////////////////
  // FILE menu options
  //
  QPopupMenu *file = new QPopupMenu( menuBar() );  CHECK_PTR( file );

  file->insertItem( "Load toas",      this, SLOT(load_toas()), CTRL+Key_T );
  file->insertItem( "Load ephemeris", this, SLOT(load_eph ()), CTRL+Key_E );
  file->insertSeparator();
  file->insertItem( "Save toas",      this, SLOT(save_toas()));
  file->insertItem( "Save ephemeris", this, SLOT(save_eph ()));

  file->insertSeparator();
  file->insertItem( "&Close", this, SLOT(closeWin()), CTRL+Key_C );
  file->insertItem( "E&xit",  qApp, SLOT(quit()),     CTRL+Key_X );

  // ///////////////////////////////////////////////////////////////////////
  // OPTIONS menu options
  //
  QPopupMenu *options = new QPopupMenu( menuBar() );  CHECK_PTR (options);

  options->insertItem ("None yet");

  // ///////////////////////////////////////////////////////////////////////
  // TEMPO menu options
  //
  QPopupMenu *tempo = new QPopupMenu( menuBar() );  CHECK_PTR (tempo);

  tempo->insertItem ("&Fit", this, SLOT(fit()) );

  // ///////////////////////////////////////////////////////////////////////
  // HELP menu options
  //
  QPopupMenu *help = new QPopupMenu( menuBar() );
  CHECK_PTR (help);
  help->insertItem( "Usage", this, SLOT(about()), CTRL+Key_H );
  if (isTopLevel())
    help->insertItem( "About Qt", this, SLOT(aboutQt()), 0);

  menuBar() -> setSeparator ( QMenuBar::InWindowsStyle );

  menuBar() -> insertItem   ( "&File",    file );
  menuBar() -> insertItem   ( "&Options", options );
  menuBar() -> insertItem   ( "&Tempo",   tempo );
  menuBar() -> insertSeparator();
  menuBar() -> insertItem   ( "&Help", help );

  if (verbose) cerr << "Rhythm::menubarConstruct () returns\n";
}

void Rhythm::about()
{
  QMessageBox::about (NULL, "Rhythm",
		      "Future replacement of psrclock?\n"
		      "A graphical user interface to TEMPO.");
}

void Rhythm::aboutQt()
{
  QMessageBox::aboutQt (NULL, "Qt Information");
}

void Rhythm::load_toas ()
{
  if (toas_modified) {
    prompt_save_toas ();
  }

  QString startName = QString::null;
  if ( !tim_filename.empty() )
    startName = tim_filename.c_str();

  QString fileName = QFileDialog::getOpenFileName ( startName,
						    "*.rthm", this);

  if ( !fileName.isNull() ) {                 // got a file name
    // DO SOMETHING with arrival_times
  }
}

void Rhythm::prompt_save_toas ()
{
  fprintf (stderr, "Rhythm::prompt_save_toas Not implemented.");
}

void Rhythm::save_toas ()
{
  fprintf (stderr, "Rhythm::save_toas Not implemented.");
}

void Rhythm::load_eph ()
{
  if (eph_modified) {
    prompt_save_eph ();
  }

  QString startName = QString::null;
  if ( !eph_filename.empty() )
    startName = eph_filename.c_str();

  QString fileName = QFileDialog::getOpenFileName ( startName,
						    "*.eph", this);

  if ( !fileName.isNull() ) {                 // got a file name
    // DO SOMETHING with ephemerides
  }
}

void Rhythm::prompt_save_eph ()
{
  fprintf (stderr, "Rhythm::prompt_save_eph Not implemented.");
}

void Rhythm::save_eph ()
{
  fprintf (stderr, "Rhythm::save_eph Not implemented.");
}

void Rhythm::fit()
{
  fprintf (stderr, "Rhythm::fit Not implemented.");
}
