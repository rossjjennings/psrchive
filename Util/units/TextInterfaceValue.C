/***************************************************************************
 *
 *   Copyright (C) 2024 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "TextInterfaceValue.h"
#include "TextInterfaceParser.h"

using namespace std;

TextInterface::Value::Value() {}

TextInterface::Value::~Value() {}

void TextInterface::Value::set_parent (Parser* p)
{
  parent.set(p);
}

void TextInterface::Value::reset_modifiers () const { tostring_precision = 0; }

void TextInterface::Value::set_modifiers (const std::string& modifiers) const
{
#ifdef _DEBUG
  cerr << "TextInterface::Value::set_modifiers " << modifiers << endl;
#endif

  tostring_precision = fromstring<unsigned> (modifiers);

#ifdef _DEBUG
  cerr << "TextInterface::Value::set_modifiers precision=" << tostring_precision << endl;
#endif
}

