#include <stdio.h>
#include <iostream>
#include "qt_value.h"

int qt_value::val_max_len = 20;
int qt_value::err_max_len = 12;
int qt_value::default_val_precision = 10;
int qt_value::default_err_precision = 3;

qt_value::qt_value (bool with_error, QWidget *parent, const char *name) :
  QHBox     (parent, name),
  value     (this),
  plusminus ("+/-", this),
  error     (this)
{
  has_error = with_error;
  val_precision = default_val_precision;
  err_precision = default_err_precision;

  QSize onechar = value.minimumSizeHint ();

  value.setMaxLength (val_max_len);
  value.resize (onechar.width()*val_max_len, onechar.height());
  value.setMinimumSize (onechar.width()*val_precision, onechar.height());

  plusminus.setMaximumSize (onechar.width()*3, onechar.height());

  error.setMaxLength (err_max_len);
  error.setMaximumSize (onechar.width()*err_max_len, onechar.height());
  error.resize (onechar.width()*err_precision, onechar.height());
  error.setMinimumSize (onechar.width()*(err_precision+1), onechar.height());

  if (!has_error) {
    error.hide();
    plusminus.hide();
  }
}
