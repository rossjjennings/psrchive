#ifndef __RHYTHM_PARAMETERS_H
#define __RHYTHM_PARAMETERS_H

#include <qtabdialog.h>

class qt_ModelOptions;

class RhythmOptions : public QTabDialog
{
  Q_OBJECT

 public:
  RhythmOptions ( QWidget *parent, const char *name);

 protected:
  void setupTab2 ();
  void setupTab3 ();
};

#endif
