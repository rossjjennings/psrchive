#include <iostream>

#include <qapplication.h>
#include <qpopupmenu.h> 
#include <qmenubar.h> 
#include <qmessagebox.h>
#include <qfiledialog.h>

#include "qt_fileParams.h"
#include "qt_editParams.h"
#include "rhythm.h"

void Rhythm::menubarConstruct ()
{
  menuBar() -> setFixedHeight ( menuBar()->fontMetrics().height() * 2 );

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
  tempo->insertItem( "Load Parameters", fitpopup, SLOT( open() ));
  saveParmsID = tempo->insertItem( "Save Parameters", fitpopup, SLOT(save()));
  tempo->setItemEnabled (saveParmsID, false);

  tempo->insertSeparator();
  // automatic fitting starts out disabled
  autofitID = tempo->insertItem( "Enable &Auto Fit",
				 this, SLOT( toglauto() ));

  // ///////////////////////////////////////////////////////////////////////
  // VERBOSITY menu options
  //
  QPopupMenu* verbosity = new QPopupMenu( menuBar() );
  quietID  = verbosity->insertItem( "Quiet" );
  mediumID = verbosity->insertItem( "Verbose" );
  noisyID  = verbosity->insertItem( "Noisy" );

  verbosity->setItemChecked( quietID, true );
  connect ( verbosity, SIGNAL( activated (int) ),
	    this, SLOT( setVerbosity(int) ) );

  // ///////////////////////////////////////////////////////////////////////
  // PLOT menu options (probably redendant)
  //
  QPopupMenu* plotter = new QPopupMenu( menuBar() );  
  plotter->insertItem( "Set Data Path", this, SLOT(setDataPath()));

  // ///////////////////////////////////////////////////////////////////////
  // OPTIONS menu options
  //
  options = new QPopupMenu( menuBar() );
  options->insertItem( "Preferences", this, SLOT(showOptions()));
  options->insertItem( "Plotter", plotter, ALT+Key_V);
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
  
  menuBar() -> insertItem   ( "File",    file );
  menuBar() -> insertItem   ( "Tempo",   tempo );
  menuBar() -> insertItem   ( "Options", options );
  menuBar() -> insertSeparator();
  menuBar() -> insertItem   ( "Help", help );

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

void Rhythm::setDataPath()
{
  QString temp = QInputDialog::getText("Set Data Path",
                                       "Please enter the path to your archives:");
  if (!temp.isEmpty())
    dataPath = temp.ascii();
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
		      "A graphical user interface to TEMPO.\n\n"
		      "   Written by: W. van Straten & A. Hotan   \n");
}

void Rhythm::aboutQt()
{
  QMessageBox::aboutQt (NULL, "Qt Information");
}

void Rhythm::load_toas ()
{
  if (toas_modified) {
    if (prompt_save_toas() != 0)
      return;
  }
  
  QString startName = QString::null;
  if ( !toa_filename.empty() )
    startName = toa_filename.c_str();

  QString fileName (QFileDialog::getOpenFileName ( startName, "*.tim", this ));

  if ( fileName.isNull() )
    return;

  load_toas ( fileName.ascii() );
}

int Rhythm::prompt_save_toas ()
{
  switch( QMessageBox::information( this, "Rhythm",
				    "You have edited the TOAs\n"
				    "Do you want to save the changes?",
				    "&Save", "&Discard", "Cancel",
				    0,      // Enter == button 0
				    2 ) ) { // Escape == button 2
  case 0: // Save clicked or Alt+S pressed or Enter pressed.
    save_toas();
    return 0;
    break;
  case 1: // Discard clicked or Alt+D pressed
    return 0;
    break;
  case 2: // Cancel clicked or Alt+C pressed or Escape pressed
    return -1;
    break;
  }
  return -1;
}

void Rhythm::save_toas ()
{
  QString startName = QString::null;
  if ( !toa_filename.empty() )
    startName = toa_filename.c_str();

  QString fileName (QFileDialog::getSaveFileName ( startName, "*.tim", this ));

  if ( fileName.isNull() )
    return;

  save_toas ( fileName.ascii() );
}

void Rhythm::setVerbosity ( int verbosityID )
{
  options->setItemChecked( quietID,  quietID  == verbosityID);
  options->setItemChecked( mediumID, mediumID == verbosityID);
  options->setItemChecked( noisyID,  noisyID  == verbosityID);

  verbose = (mediumID == verbosityID || noisyID  == verbosityID);
  vverbose = (noisyID == verbosityID);

  setClassVerbose (vverbose);

  if (vverbose)
    cerr << "rhythm: very verbose on" << endl;
  else if (verbose)
    cerr << "rhythm: verbose on" << endl;
}









