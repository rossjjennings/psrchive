/* $Source: /cvsroot/psrchive/psrchive/Util/qtutil/qt_Angle.h,v $
   $Revision: 1.6 $
   $Date: 2005/03/02 05:35:42 $
   $Author: sord $ */

#ifndef __QT_Angle_H
#define __QT_Angle_H

#include "qt_value.h"
#include "Angle.h"

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

  void setWrap (double wrap) {valset.setWrapPoint(wrap);};

 protected:
  bool   hms;
  Angle  valset;

 protected slots:
  void value_Entered_CB ();
};

#endif
