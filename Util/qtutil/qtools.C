#include <iostream>

#include "qtools.h"

bool qevent_trace::eventFilter (QObject *k, QEvent *e)
{
  cerr << "QEventTrace: " << k->name() << " receives ";

  switch (e->type()) {
  case QEvent::None:
    cerr << "NONE" << endl;
    break;
  case QEvent::Timer:
    cerr << "TIMER" << endl;
    break;
  case QEvent::MouseButtonPress:
    cerr << "MOUSE BUTTON PRESS" << endl;
    break;
  case QEvent::MouseButtonRelease:
    cerr << "MOUSE BUTTON RELEASE" << endl;
    break;
  case QEvent::MouseButtonDblClick:
    cerr << "DOUBLE CLICK" << endl;
    break;
  case QEvent::MouseMove:
    cerr << "MOUSE MOVE" << endl;
    break;
  case QEvent::KeyPress:
    cerr << "KEY PRESS" << endl;
    break;
  case QEvent::KeyRelease:
    cerr << "KEY RELEASE" << endl;
    break;
  case QEvent::FocusIn:
    cerr << "FOCUS IN" << endl;
    break;
  case QEvent::FocusOut:
    cerr << "FOCUS OUT" << endl;
    break;  
  case QEvent::Enter:
    cerr << "ENTER" << endl;
    break;
  case QEvent::Leave:
    cerr << "LEAVE" << endl;
    break;
  case QEvent::Paint:
    cerr << "PAINT" << endl;
    break;
  case QEvent::Move:
    cerr << "MOVE" << endl;
    break;
  case QEvent::Resize:
    cerr << "RESIZE" << endl;
    break;
  case QEvent::Create:
    cerr << "CREATE" << endl;
    break;
  case QEvent::Destroy:
    cerr << "DESTROY" << endl;
    break;
  case QEvent::Show:
    cerr << "SHOW" << endl;
    break;
  case QEvent::Hide:
    cerr << "HIDE" << endl;
    break;
  case QEvent::Close:
    cerr << "CLOSE" << endl;
    break;
  case QEvent::Quit:
    cerr << "QUIT" << endl;
    break;
  case QEvent::Accel:
    cerr << "ACCEL" << endl;
    break;
  case QEvent::Wheel:
    cerr << "WHEEL" << endl;
    break;
  case QEvent::AccelAvailable:
    cerr << "AccelAvailable" << endl;
    break;
  case QEvent::Clipboard:
    cerr << "Clipboard" << endl;
    break;
  case QEvent::SockAct:
    cerr << "SockAct" << endl;
    break;
  case QEvent::DragEnter:
    cerr << "DragEnter" << endl;
    break;
  case QEvent::DragMove:
    cerr << "DragMove" << endl;
    break;
  case QEvent::DragLeave:
    cerr << "DragLeave" << endl;
    break;
  case QEvent::Drop:
    cerr << "Drop" << endl;
    break;
  case QEvent::DragResponse:
    cerr << "DragResponse" << endl;
    break;
  case QEvent::ChildInserted:
    cerr << "CHILD INSERTED" << endl;
    break;
  case QEvent::ChildRemoved:
    cerr << "CHILD REMOVED" << endl;
    break;
  case QEvent::LayoutHint:
    cerr << "LAYOUT HINT" << endl;
    break;
  case QEvent::ActivateControl:
    cerr << "ACTIVATE CONTROL" << endl;
    break;
  case QEvent::DeactivateControl:
    cerr << "DE-ACTIVATE CONTROL" << endl;
    break;
  case QEvent::User:
    cerr << "USER" << endl;
    break;
  default:
    cerr << "INVALID!" << endl;
    break;
  }
  return false;  // standard event processing
}

ostream & operator<< (ostream & s, const QSize & sz) 
{
  if (sz.isValid ())
    return s << "(" << sz.width() << "," << sz.height() << ")";
  else
    return s << "(invalid)";
}
