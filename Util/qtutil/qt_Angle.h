/* $Source: /cvsroot/psrchive/psrchive/Util/qtutil/qt_Angle.h,v $
   $Revision: 1.2 $
   $Date: 1999/11/02 09:00:48 $
   $Author: straten $ */

#ifndef __QT_Angle_H
#define __QT_Angle_H

#include "qt_value.h"
#include "angle.h"

class qt_Angle : public qt_value
{
  Q_OBJECT

 public:
  qt_Angle (bool error=false, QWidget *parent=NULL, const char *name=NULL);

  // error given in days
  int setAngle (const Angle& angle, double error=0.0);
  int getAngle (Angle* angle, double* error);

  void displayHMS () { hms = true; };
  void displayDMS () { hms = false; };

 protected:
  bool   hms;
  Angle  valset;

 protected slots:
  void value_Entered_CB ();
};

#endif
