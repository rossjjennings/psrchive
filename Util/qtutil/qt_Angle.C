#include <stdio.h>
#include <iostream>
#include "qt_Angle.h"

qt_Angle::qt_Angle (bool with_error, QWidget *parent, const char *name) :
  qt_value (with_error, parent, name)
{
  connect (&value, SIGNAL(returnPressed()), this, SLOT(value_Entered_CB()));
}

int qt_Angle::setAngle (const Angle& angle, double err)
{
  if (hms)
    value.setText (angle.getHMS(val_precision).data());
  else
    value.setText (angle.getDMS(val_precision).data());

  valset = angle;

  if (err != 0.0) {
    if (!has_error) {
      plusminus.show();
      error.show();
      has_error = true;
    }
    char str_data [80];
    sprintf (str_data, "%.*le", err_precision, err);
    error.setText (str_data);
  }
  return 0;
}

int qt_Angle::getAngle (Angle* angle, double* err)
{
  if (angle != NULL)
    *angle = valset;

  if (err != NULL) {
    *err = 0.0;
    if (has_error &&
	sscanf (error.text().ascii(), "%lf", err) != 1)
      *err = 0.0;
  }
  return 0;
}

void qt_Angle::value_Entered_CB ()
{
  int retval = 0;
  Angle newval;

  if (hms)
    retval = newval.setHMS (value.text().ascii());
  else
    retval = newval.setDMS (value.text().ascii());

  if (retval < 0)
    cerr << "gtk_Angle:: invalid angle:" << value.text() << endl;
  else
    valset = newval;
  
  setAngle (valset);
}
