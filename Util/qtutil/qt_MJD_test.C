#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include "qt_MJD.h"

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  MJD today (time (NULL));

  qt_MJD display;
  display.setMJD (today);

  a.setMainWidget( &display );
  display.show();

  return a.exec();
}
