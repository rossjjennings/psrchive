/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ConfigurableScrollBar.h"
#include "RangePolicy.h"

#include <qscrollbar.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qhbox.h>
#include <qcombobox.h>

#include <iostream>
using namespace std;

ConfigurableScrollBar::ConfigurableScrollBar (QWidget *parent, 
					      const char *name)
  : QVBox (parent, name)
{
  QHBox* hbox = new QHBox (this);

  new QLabel ("Name:", hbox);
  parameter_name = new QLineEdit (hbox);

  connect (parameter_name, SIGNAL(lostFocus()),
	   this, SLOT(parameter_name_CB()));

  new QLabel ("Min:", hbox);
  minimum_value = new QLineEdit (hbox);

  connect (minimum_value, SIGNAL(lostFocus()),
	   this, SLOT(minimum_value_CB()));

  new QLabel ("Max:", hbox);
  maximum_value = new QLineEdit (hbox);

  connect (maximum_value, SIGNAL(lostFocus()),
	   this, SLOT(maximum_value_CB()));

  new QLabel ("Type:", hbox);
  conversion_type = new QComboBox (hbox);

  conversion_type->insertItem ("integer");
  conversion_type->insertItem ("real");

  connect (conversion_type, SIGNAL( highlighted(const QString&) ),
	   this, SLOT( conversion_type_CB(const QString&) ));

  scroll_bar = new QScrollBar (this);
  scroll_bar->setOrientation (Qt::Horizontal);

  connect (scroll_bar, SIGNAL( valueChanged(int) ),
	   this, SLOT( scroll_bar_CB(int) ));

  range_policy = new IntegerRangePolicy( scroll_bar->minValue(),
					 scroll_bar->maxValue() );

  minimum_value->setText( range_policy->get_output_min().c_str() );
  maximum_value->setText( range_policy->get_output_max().c_str() );

  setFocusPolicy (QWidget::StrongFocus);
}


void ConfigurableScrollBar::parameter_name_CB ()
{
  current_parameter_name = parameter_name->text().ascii();
}

void ConfigurableScrollBar::minimum_value_CB ()
{
  range_policy->set_output_min( minimum_value->text().ascii() );
  minimum_value->setText( range_policy->get_output_min().c_str() );

  scroll_bar->setMinValue( range_policy->get_input_min() );
}

void ConfigurableScrollBar::maximum_value_CB ()
{
  range_policy->set_output_max( maximum_value->text().ascii() );
  maximum_value->setText( range_policy->get_output_max().c_str() );

  scroll_bar->setMaxValue( range_policy->get_input_max() );
}

void ConfigurableScrollBar::conversion_type_CB (const QString& selected)
{
  if (selected == "real")
    set_range_policy( new RealRangePolicy );
  else
    set_range_policy( new IntegerRangePolicy( scroll_bar->minValue(),
					      scroll_bar->maxValue() ) );
}

void ConfigurableScrollBar::set_range_policy (RangePolicy* policy)
{
  if (range_policy)
  {
    policy->set_output_min( range_policy->get_output_min() );
    policy->set_output_max( range_policy->get_output_max() );
    delete range_policy;
  }

  range_policy = policy;

  minimum_value->setText( range_policy->get_output_min().c_str() );
  maximum_value->setText( range_policy->get_output_max().c_str() );

  scroll_bar->setMinValue( range_policy->get_input_min() );
  scroll_bar->setMaxValue( range_policy->get_input_max() );
}

void ConfigurableScrollBar::scroll_bar_CB (int value)
{
  output (current_parameter_name + "=" + range_policy->get_output (value));
}

