#include <iostream>

#include <qapplication.h>
#include <qpopupmenu.h> 
#include <qmenubar.h> 
#include <qmessagebox.h>
#include <qfiledialog.h>

#include "qt_fileParams.h"
#include "qt_editParams.h"
#include "tempo++.h"
#include "rhythm.h"

void Rhythm::menubarConstruct ()
{
  // ///////////////////////////////////////////////////////////////////////
  // FILE menu options
  //
  QPopupMenu *file = new QPopupMenu( menuBar() );  CHECK_PTR( file );

  file->insertItem( "&Load TOAs",      this, 
		    SLOT( load_toas() ), ALT+Key_L );
  file->insertItem( "&Save TOAs",      this, 
		    SLOT( save_toas() ), ALT+Key_S );

  file->insertSeparator();
  file->insertItem( "&Close", this, SLOT(closeWin()), ALT+Key_C );
  file->insertItem( "E&xit",  qApp, SLOT(quit()),     ALT+Key_X );

  // ///////////////////////////////////////////////////////////////////////
  // TEMPO menu options
  //
  tempo = new QPopupMenu( menuBar() );  CHECK_PTR (tempo);

  fitID = tempo->insertItem( "&Fit", this, SLOT( fit() ));
  tempo->setItemEnabled (fitID, false);
  dispID = tempo->insertItem( "&Display Parameters", this, SLOT( togledit() ));
  tempo->setItemEnabled (dispID, false);

  tempo->insertSeparator();
  tempo->insertItem( "Load Paremeters", fitpopup, SLOT( open() ));
  saveParmsID = tempo->insertItem( "Save Parameters", fitpopup, SLOT(save()));
  tempo->setItemEnabled (saveParmsID, false);

  tempo->insertSeparator();
  // automatic fitting starts out enabled
  autofitID = tempo->insertItem( "Disable &Auto Fit",
				 this, SLOT( toglauto() ));

  // ///////////////////////////////////////////////////////////////////////
  // OPTIONS menu options
  //
  verbosity = new QPopupMenu( menuBar() );  CHECK_PTR (verbosity);
  quietID  = verbosity->insertItem( "Quiet", this, SLOT( quiet() ));
  mediumID = verbosity->insertItem( "Verbose", this, SLOT( medium() ));
  noisyID  = verbosity->insertItem( "Noisy", this, SLOT( noisy() ));

  verbosity->setItemChecked( quietID, true );

  options = new QPopupMenu( menuBar() );  CHECK_PTR (options);
  options->insertSeparator();
  options->insertItem( "&Verbosity", verbosity, ALT+Key_V);

  // ///////////////////////////////////////////////////////////////////////
  // HELP menu options
  //
  QPopupMenu *help = new QPopupMenu( menuBar() );
  CHECK_PTR (help);
  help->insertItem( "&Usage", this, SLOT(about()), CTRL+Key_H );
  if (isTopLevel())
    help->insertItem( "About Qt", this, SLOT(aboutQt()), 0);

  menuBar() -> setSeparator ( QMenuBar::InWindowsStyle );

  menuBar() -> insertItem   ( "&File",    file );
  menuBar() -> insertItem   ( "&Tempo",   tempo );
  menuBar() -> insertItem   ( "&Options", options );
  menuBar() -> insertSeparator();
  menuBar() -> insertItem   ( "&Help", help );

  if (verbose) cerr << "Rhythm::menubarConstruct () returns\n";
}

void Rhythm::togledit()
{
  if (!fitpopup)
    return;

  if (fitpopup->isVisible()) {
    fitpopup->hide();
    tempo->setItemEnabled (dispID, TRUE);
  }
  else {
    fitpopup->show();
    tempo->setItemEnabled (dispID, FALSE);
  }
}

void Rhythm::toglauto()
{
  QString text;
  if (autofit)
    text = "Enable";
  else
    text = "Disable";
  text += " &Auto Fit";

  tempo->changeItem ( autofitID, text );
  autofit = !autofit;

  if (verbose) {
    cerr << "Rhythm: Automatic Fitting ";
    if (autofit)
      cerr << "enabled.\n";
    else
      cerr << "disabled.\n";
    cerr << endl;
  }
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

  QString fileName (QFileDialog::getOpenFileName ( startName, "*.tim", this ));

  if ( fileName.isNull() )
    return;

  load_toas ( fileName.ascii() );
}

void Rhythm::prompt_save_toas ()
{
  fprintf (stderr, "Rhythm::prompt_save_toas Not implemented.");
}

void Rhythm::save_toas ()
{
  fprintf (stderr, "Rhythm::save_toas Not implemented.");
}

void Rhythm::quiet ()
{
  options->setItemChecked( quietID, true );
  options->setItemChecked( mediumID, false );
  options->setItemChecked( noisyID, false );

  verbose = false;
  vverbose = false;
  qt_fileParams::verbose = 0;
  qt_editParams::verbose = 0;
  xmp_manager::verbose = 0;
  pg_manager::verbose = 0;
  xyplot::verbose = 0;
  Tempo::verbose = 0;
}

void Rhythm::medium ()
{
  quiet ();
  options->setItemChecked( quietID, false );
  options->setItemChecked( mediumID, true );
  verbose = true;
  cerr << "rhythm: verbose on" << endl;
}

void Rhythm::noisy ()
{
  options->setItemChecked( quietID, false );
  options->setItemChecked( mediumID, false );
  options->setItemChecked( noisyID, true );

  verbose = true;
  vverbose = true;
  qt_fileParams::verbose = 1;
  qt_editParams::verbose = 1;
  xmp_manager::verbose = 1;
  pg_manager::verbose = 1;
  xyplot::verbose = 1;
  Tempo::verbose = 1;
  cerr << "rhythm: very verbose on" << endl;
}
