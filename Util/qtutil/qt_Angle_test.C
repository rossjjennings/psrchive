#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include "qt_Angle.h"

int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  qt_Angle display (true);

  a.setMainWidget( &display );
  display.show();

  return a.exec();
}
