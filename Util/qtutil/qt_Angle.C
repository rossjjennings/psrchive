#include <stdio.h>
#include <iostream>
#include "qt_Angle.h"

qt_Angle::qt_Angle (bool with_error, QWidget *parent, const char *name) :
  qt_value (with_error, parent, name)
{
  setAngle ();
  connect (&value, SIGNAL(returnPressed()), this, SLOT(value_Entered_CB()));
}

void qt_Angle::setAngle (const Angle& angle)
{
  if (hms)
    value.setText (angle.getHMS(val_precision).c_str());
  else
    value.setText (angle.getDMS(val_precision).c_str());

  valset = angle;
}

void qt_Angle::value_Entered_CB ()
{
  int retval = 0;
  Angle newval;

  if (hms)
    retval = newval.setHMS (value.text().ascii());
  else
    retval = newval.setDMS (value.text().ascii());

  if (retval < 0) {
    if (verbose)
      cerr << "gtk_Angle:: invalid angle:" << value.text() << endl;
    newval = valset;
  }
  setAngle (newval);
}
