#include <stdio.h>
#include <stdlib.h>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qxt.h>

#include "rhythm.h"

int Rhythm::verbose = 0;

Rhythm::Rhythm (QWidget* parent, int argc, char** argv) :
  QMainWindow (parent, "Rhythm"),
  plot_manager (this, 800, 700)
{
  fitpopup = NULL;

  menubarConstruct(); 

  setCentralWidget (&plot_manager);

  command_line_parse (argc, argv);
}

int main (int argc, char** argv)
{
  QXtApplication app (argc, argv, "RhythmApp");
  Rhythm rhythm (0, argc, argv);
  
  app.setMainWidget (&rhythm);
  rhythm.show();
  return app.exec();
}
