#ifndef __QT_FILEEPH_H
#define __QT_FILEEPH_H

#include <qfiledialog.h>

#include "psrParams.h"

class qt_fileParams : public QWidget
{
  Q_OBJECT
 public:
  qt_fileParams ( const QString& startname = QString::null, QWidget * parent=0 );
  void open ( psrParams* eph );
  void save ( const psrParams& eph );

  static bool verbose;

 protected slots:
  void chosen ( const QString & );

 protected:
  QFileDialog* window;
  QString fileName;
};

#endif
