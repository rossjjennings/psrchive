#ifndef __QT_FILEEPH_H
#define __QT_FILEEPH_H

#include <string>

#include <qfiledialog.h>

class psrParams;

class qt_fileParams : public QFileDialog
{
  Q_OBJECT

 public:
  static bool verbose;
  qt_fileParams (const QString& startname = QString::null, QWidget* parent=0);

  // open() and save() return zero if the user cancels, 1 otherwise
  int open ( psrParams* eph );       // also returns zero if no data is loaded
  int save ( const psrParams& eph );

  // string filename () { return string (fileName); };
 protected slots:
  // called when the user has chosen a filename
  void chosen ( const QString & );

 protected:
  string fileName;
};

#endif
