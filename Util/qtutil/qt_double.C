#include <stdio.h>
#include <iostream>
#include "qt_double.h"

qt_double::qt_double (bool with_error, QWidget *parent, const char *name) :
  qt_value (with_error, parent, name)
{
  connect (&value, SIGNAL(returnPressed()), this, SLOT(value_Entered_CB()));
}

int qt_double::setdouble (double d_val, double err)
{
  char str_data [80];
  sprintf (str_data, "%.*le", val_precision, d_val);
  value.setText (str_data);
  valset = d_val;

  if (has_error) {
    sprintf (str_data, "%.*le", err_precision, err);
    error.setText (str_data);
  }
  return 0;
}

int qt_double::getdouble (double* d_val, double* err)
{
  if (d_val != NULL)
    *d_val = valset;

  if (err != NULL) {
    *err = 0.0;
    if (has_error &&
	sscanf (error.text().ascii(), "%lf", err) != 1)
      *err = 0.0;
  }
  return 0;
}

void qt_double::value_Entered_CB ()
{
  double newval;

  if (sscanf (value.text().ascii(), "%lf", &newval) != 1)
    cerr << "gtk_double:: invalid double:" << value.text() << endl;
  else
    valset = newval;
  
  setdouble (valset);
}
