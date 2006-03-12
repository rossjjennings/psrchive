#include "plotItem.h"

Pulsar::plotItem::plotItem(QWidget* parent, Pulsar::Plot* figPtr, 
			   const QString& name, const QString& desc)
  : QRadioButton(name, parent)
{
  QWhatsThis::add(this, desc);
  figStyle = figPtr;
}
