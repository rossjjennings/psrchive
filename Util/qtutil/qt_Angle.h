/* $Source: /cvsroot/psrchive/psrchive/Util/qtutil/qt_Angle.h,v $
   $Revision: 1.4 $
   $Date: 2004/02/28 06:58:48 $
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

  void displayHMS ();
  void displayDMS ();

 protected:
  bool   hms;
  Angle  valset;

 protected slots:
  void value_Entered_CB ();
};

#endif
