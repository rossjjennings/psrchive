#include <stdio.h>
#include <stdlib.h>

#include <gtk--/main.h>

#include "rhythm.h"

int main (int argc, char** argv)
{
  Gtk_Main gmain ( &argc, &argv );

  rhythm rmain ( argc, argv );

  rmain.show_all();

  gmain.run();
  return 0;
}

int rhythm::verbose = 1;

rhythm::rhythm (int argc, char** argv)
{ 
  save_tim = save_eph = NULL;
  fileselect = NULL;

  main_window.set_usize (800, 700);
  add (&main_window);

  menubarConstruct(); 
  main_window.add (menubar);

  plots.set_usize (880, 680);
  main_window.add (&plots);
  
  show_all();
}
