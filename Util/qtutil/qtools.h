#ifndef __QTOOLS_H
#define __QTOOLS_H

#include <qsize.h>
#include <qobject.h>
#include <qevent.h>

//
// a nice way of outputting QSize
//
// eg.
// QSize size ( width, height );
// cerr << "Size: " << size << endl;
//
ostream & operator<< (ostream & s, const QSize & sz);

//
// a nice way to trace events as delivered to a widget
//
// eg.
// QWidget any_widget;
// qevent_trace tracer;
// any_widget.installEventFilter ( &tracer );
//
class qevent_trace : public QObject
{
 protected:
  bool  eventFilter( QObject *, QEvent * );
};

#endif
