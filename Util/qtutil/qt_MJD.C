#include <iostream>
#include "qt_MJD.h"

qt_MJD::qt_MJD (bool with_error, QWidget *parent, const char *name) :
  qt_value (with_error, parent, name)
{
  connect (&value, SIGNAL(returnPressed()), this, SLOT(value_Entered_CB()));
}

int qt_MJD::setMJD (const MJD& mjd, double err)
{
  char str_data [80];
  value.setText (mjd.printdays(val_precision).data());
  valset = mjd;

  if (err != 0.0) {
    if (!has_error) {
      plusminus.show();
      error.show();
      has_error = true;
    }
    sprintf (str_data, "%.*le", err_precision, err);
    error.setText (str_data);
  }
  return 0;
}

int qt_MJD::getMJD (MJD* mjd, double* err)
{
  if (mjd != NULL)
    *mjd = valset;

  if (err != NULL) {
    *err = 0.0;
    if (has_error &&
	sscanf (error.text().ascii(), "%lf", err) != 1)
      *err = 0.0;
  }
  return 0;
}

void qt_MJD::value_Entered_CB ()
{
  MJD newval;
  if (newval.Construct (value.text().ascii()) < 0)
    cerr << "qt_MJD:: invalid mjd:" << value.text() << endl;
  else
    valset = newval;

  setMJD (valset);
}
