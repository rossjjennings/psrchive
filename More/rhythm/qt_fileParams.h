#ifndef __QT_FILEEPH_H
#define __QT_FILEEPH_H

#include <qfiledialog.h>

class psrParams;

class qt_fileParams : public QWidget
{
  Q_OBJECT
 public:
  qt_fileParams (const QString& startname = QString::null, QWidget* parent=0);

  // returns 0 if user cancels, 1 otherwise
  int open ( psrParams* eph );

  // returns 0 if user cancels, 1 otherwise
  int save ( const psrParams& eph );

  // string filename () { return string (fileName); };

  static bool verbose;

 protected slots:
  void chosen ( const QString & );

 protected:
  QFileDialog* window;
  QString fileName;
};

#endif
