#include <qvbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qdatetime.h>

#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <qlistbox.h>
#include <qfileinfo.h>
#include <qapplication.h>

#include "Options.h"
#include "rhythm.h"
#include "residual.h"


RhythmOptions::RhythmOptions( QWidget *parent, const char *name )
    : QTabDialog ( parent, name )
{
  setupPlot();
  setupTab2();
  setupTab3();
  
  // connect( this, SIGNAL( applyButtonPressed() ), qApp, SLOT( quit() ) );
}

static QFileInfo fileinfo ("0437-4715.par");

void RhythmOptions::setupTab2()
{
  QVBox *tab1 = new QVBox( this );
  tab1->setMargin( 5 );
  
  (void)new QLabel( "Filename:", tab1 );
  QLineEdit *fname = new QLineEdit( "poop", tab1 );
  fname->setFocus();
  
  (void)new QLabel( "Path:", tab1 );
  QLabel *path = new QLabel( fileinfo.dirPath( TRUE ), tab1 );
  path->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  (void)new QLabel( "Size:", tab1 );
  QLabel *size = new QLabel( QString( "%1 KB" ).arg( fileinfo.size() ), tab1 );
  size->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  (void)new QLabel( "Last Read:", tab1 );
  QLabel *lread = new QLabel( fileinfo.lastRead().toString(), tab1 );
  lread->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  (void)new QLabel( "Last Modified:", tab1 );
  QLabel *lmodif = new QLabel( fileinfo.lastModified().toString(), tab1 );
  lmodif->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  addTab( tab1, "General" );
}

void RhythmOptions::setupPlot()
{
  QVBox *plotTab = new QVBox( this );
  plotTab->setMargin( 5 );

  QHBox *axisTab = new QHBox( plotTab );

  QRadioButton *radbut;

  QButtonGroup *xaxis = new QButtonGroup( 1, QGroupBox::Horizontal, 
					  "X-Axis", axisTab );
  xaxis -> setRadioButtonExclusive (true);
  connect( xaxis, SIGNAL( clicked(int) ), this, SLOT( setXAxis(int) ) );

  radbut = new QRadioButton( "MJD", xaxis );
  xaxis -> insert ( radbut, residual::Mjd );
  radbut->setChecked( residual::xtype == residual::Mjd );

  radbut = new QRadioButton( "Binary Phase", xaxis );
  xaxis -> insert ( radbut, residual::BinaryPhase );
  radbut->setChecked( residual::xtype == residual::BinaryPhase );

  // enum   plot { BinaryPhase, Mjd, Seconds, Turns };
 
  QButtonGroup *yaxis = new QButtonGroup( 1, QGroupBox::Horizontal, 
					  "Y-Axis", axisTab );
  yaxis -> setRadioButtonExclusive (true);
  connect( yaxis, SIGNAL( clicked(int) ), this, SLOT( setYAxis(int) ) );

  radbut = new QRadioButton( "Seconds", yaxis );
  yaxis -> insert ( radbut, residual::Seconds );
  radbut->setChecked( residual::ytype == residual::Seconds );

  radbut = new QRadioButton( "Turns", yaxis );
  yaxis -> insert ( radbut, residual::Turns );
  radbut->setChecked( residual::ytype == residual::Turns );

  QButtonGroup *bg2 = new QButtonGroup( 2, QGroupBox::Horizontal, 
					"Owner", plotTab );
  
  (void)new QLabel( "Owner", bg2 );
  QLabel *owner = new QLabel( fileinfo.owner(), bg2 );
  owner->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  (void)new QLabel( "Group", bg2 );
  QLabel *group = new QLabel( fileinfo.group(), bg2 );
  group->setFrameStyle( QFrame::Panel | QFrame::Sunken );
  
  addTab( plotTab, "Plot" );
}

void RhythmOptions::setupTab3()
{
  QVBox *tab3 = new QVBox( this );
  tab3->setMargin( 5 );
  tab3->setSpacing( 5 );
  
  (void)new QLabel( QString( "Open %1 with:" ).arg( "poop" ), tab3 );
  
  QListBox *prgs = new QListBox( tab3 );
  for ( unsigned int i = 0; i < 30; i++ ) {
    QString prg = QString( "Application %1" ).arg( i );
    prgs->insertItem( prg );
  }
  prgs->setCurrentItem( 3 );
  
  (void)new QCheckBox( QString( "Open files with the extension '%1' always with this application" ).arg( fileinfo.extension() ), tab3 );
  
  addTab( tab3, "Applications" );
}

void RhythmOptions::setXAxis ( int residual_plot )
{
  if (Rhythm::verbose)
    cerr << "RhythmOptions::setXAxis " << residual_plot << endl;
  residual::xtype = residual_plot; 
}

void RhythmOptions::setYAxis ( int residual_plot )
{
  if (Rhythm::verbose)
    cerr << "RhythmOptions::setYAxis " << residual_plot << endl;
  residual::ytype = residual_plot;
}
