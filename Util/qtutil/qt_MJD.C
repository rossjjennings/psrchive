#include <iostream>
#include "qt_MJD.h"

qt_MJD::qt_MJD (bool with_error, QWidget *parent, const char *name) :
  qt_value (with_error, parent, name)
{
  setMJD();
  connect (&value, SIGNAL(returnPressed()), this, SLOT(value_Entered_CB()));
}

void qt_MJD::setMJD (const MJD& mjd)
{
  value.setText (mjd.printdays(val_precision).c_str());
  valset = mjd;
}

void qt_MJD::value_Entered_CB ()
{
  MJD newval;
  if (newval.Construct (value.text().ascii()) < 0) {
    if (MJD::verbose)
      cerr << "qt_MJD:: invalid mjd:" << value.text() << endl;
    newval = valset;
  }
  setMJD (newval);
}
