#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qxt.h>

#include "rhythm.h"

int Rhythm::verbose = 1;


Rhythm::Rhythm (int argc, char** argv) :
  QWidget (0, "Rhythm"),
  main_window (this,    "main window"),
  menu   (&main_window, "menubar"),
  plots  (&main_window, "plots")
{
  // save_tim = save_eph = NULL;
  fitpopup = NULL;

  main_window.setMinimumSize (800, 700);

  menubarConstruct(); 

  plots.setMinimumSize (880, 680);
}

int main (int argc, char** argv)
{
  QXtApplication app (argc, argv, "Rhythm");

  Rhythm rhythm (argc, argv);

  app.setMainWidget (&rhythm);

  rhythm.show();

  return app.exec();
}

