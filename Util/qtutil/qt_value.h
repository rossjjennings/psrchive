#ifndef __QT_VALUE_H
#define __QT_VALUE_H

#include <qhbox.h>
#include <qlineedit.h>
#include <qlabel.h>

class qt_value : public QHBox
{
  Q_OBJECT
 public:
  qt_value (bool error=false, QWidget *parent=NULL, const char *name=NULL);

  static int val_max_len;
  static int err_max_len;
  static int default_val_precision;
  static int default_err_precision;

  int val_precision;
  int err_precision;

 protected:
  // Widgets
  QLineEdit value;
  QLabel    plusminus;
  QLineEdit error;

  bool has_error;
};

#endif
