#ifndef __RHYTHM_PARAMETERS_H
#define __RHYTHM_PARAMETERS_H

#include <qtabdialog.h>

class RhythmOptions : public QTabDialog
{
  Q_OBJECT

 public:
  RhythmOptions ( QWidget *parent, const char *name);

 public slots:
  void setXAxis ( int residual_plot );
  void setYAxis ( int residual_plot );

 protected:
  void setupPlot ();
  void setupTab2 ();
  void setupTab3 ();
};

#endif
