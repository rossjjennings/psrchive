/* $Source: /cvsroot/psrchive/psrchive/Util/qtutil/qt_Angle.h,v $
   $Revision: 1.3 $
   $Date: 1999/12/23 02:05:46 $
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
  void setAngle ( const Angle& angle = Angle() );
  Angle getAngle () const { return valset; };

  void displayHMS () { hms = true;  setAngle(valset); };
  void displayDMS () { hms = false; setAngle(valset); };

 protected:
  bool   hms;
  Angle  valset;

 protected slots:
  void value_Entered_CB ();
};

#endif
