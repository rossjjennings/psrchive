/* $Source: /cvsroot/psrchive/psrchive/Util/qtutil/qt_MJD.h,v $
   $Revision: 1.3 $
   $Date: 1999/12/23 02:05:48 $
   $Author: straten $ */

#ifndef __QT_MJD_H
#define __QT_MJD_H

#include "qt_value.h"
#include "MJD.h"

class qt_MJD : public qt_value
{
  Q_OBJECT

 public:
  qt_MJD (bool with_error=false, QWidget *parent=NULL, const char *name=NULL);

  void setMJD ( const MJD& mjd = MJD() );
  MJD getMJD () const { return valset; };

 protected:
  MJD  valset;

 protected slots:
  void value_Entered_CB ();
};

#endif
