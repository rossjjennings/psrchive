#ifndef __QT_DOUBLE_H
#define __QT_DOUBLE_H

#include "qt_value.h"

class qt_double : public qt_value
{
  Q_OBJECT

 public:
  qt_double (bool error=false, QWidget *parent=NULL, const char *name=NULL);

  // error given in days
  int setdouble (double d_val, double error=0.0);
  int getdouble (double* d_val, double* error);

 protected:
  double  valset;

 protected slots:
  void value_Entered_CB ();
};

#endif
