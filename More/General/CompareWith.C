/***************************************************************************
 *
 *   Copyright (C) 2021 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "Pulsar/CompareWith.h"
#include "BinaryStatistic.h"

using namespace Pulsar;
using namespace std;

CompareWith::CompareWith ()
{
  data = 0;
  statistic = 0;
  primary = 0;
  compare = 0;
  transpose = false;
}

void CompareWith::set_statistic (BinaryStatistic* stat)
{
  statistic = stat;
}

void CompareWith::set_data (HasArchive* _data)
{
  data = _data;
}

void CompareWith::set_primary (unsigned n,
			       void (HasArchive::*func) (Index))
{
  nprimary = n;
  primary = func;
}

void CompareWith::set_compare (unsigned n,
			       void (HasArchive::*func) (Index))
{
  ncompare = n;
  compare = func;
}

void CompareWith::set_transpose (bool f)
{
  transpose = f;
}

void CompareWith::set (ndArray<2,double>& result,
		       unsigned iprimary, unsigned icompare,
		       double value)
{
#if _DEBUG
  cerr << "CompareWith::set i=" << iprimary << " j=" << icompare
       << " val=" << value << endl;
#endif
  
  if (transpose)
    result[icompare][iprimary] = value;
  else
    result[iprimary][icompare] = value;
}

void CompareWith::check (const char* context)
{
  if (!data)
    throw Error (InvalidState, context, "archive data not set");

  if (!statistic)
    throw Error (InvalidState, context, "binary statistic not set");

  if (!primary)
    throw Error (InvalidState, context, "primary index method not set");

  if (!compare)
    throw Error (InvalidState, context, "compare index method not set");
}
