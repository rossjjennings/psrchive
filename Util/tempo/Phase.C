/***************************************************************************
 *
 *   Copyright (C) 1999 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include "Phase.h"

double Phase::rounding_threshold = 1e-9;

const Phase Phase::zero;

void Phase::settle ()
{
  if (turns>0 && fturns < -rounding_threshold) {
    fturns += 1.0;
    turns--;
  }
  if (turns<0 && fturns > rounding_threshold) {
    fturns -= 1.0;
    turns++;
  }
}

Phase::Phase (double tns)
{
  turns = (int64) tns;
  fturns = tns - double (turns);
  settle ();
}

Phase::Phase (int64 tns, double ftns) 
{
  int64 iturns = (int64) ftns;
  turns = tns + iturns;
  fturns = ftns - double (iturns);
  settle ();
}

double Phase::in_turns() const {
  return double(turns)+fturns;
}

int64 Phase::intturns() const {
  return turns;
}

double Phase::fracturns() const {
  return fturns;
}

std::string Phase::strprint(int precision) const
{
  if (precision>DBL_DIG) {
    std::cerr << "Phase::strprint warning: precision of " << precision 
	 << " exceeds that of a double" << std::endl;
    std::cerr << "- truncating to a precision of " << DBL_DIG << std::endl;
  }

  char ftn[30];
  // I64 is defined in environ.h.
  sprintf(ftn, I64, turns);

  std::string s = ftn;
  sprintf(ftn, "%.*lf", precision, fturns);

  if (fturns>=0)
    s += &(ftn[1]);
  else
    s += &(ftn[2]);

  return s;
}

Phase& Phase::operator = (double turns)
{
  *this = Phase(turns);
  return *this;
}

Phase& Phase::operator = (const Phase &in_Phase)
{
  if (this != &in_Phase) {
    turns = in_Phase.turns;
    fturns = in_Phase.fturns;
  }
  return *this;
}

Phase operator + (const Phase &p1, const Phase &p2) {
  return Phase(p1.turns + p2.turns,
	       p1.fturns + p2.fturns); 
}

Phase operator - (const Phase &p1, const Phase &p2) {
  return Phase(p1.turns - p2.turns,
	       p1.fturns - p2.fturns); 
}

Phase operator - (const Phase &p) {
  return Phase( -p.turns, -p.fturns );
}


// period is assumed to be given in seconds
MJD operator * (const Phase &p1, double period) 
{
  const int64 secs_per_day = 86400;

  double bigseconds = double (p1.turns) * period;
  double smallseconds = p1.fturns * period;

  int64 b_seconds = int64 (bigseconds);
  int64 s_seconds = int64 (smallseconds);

  double b_fracsec = bigseconds - double (b_seconds);
  double s_fracsec = smallseconds - double (s_seconds);

  int b_days = int (b_seconds / secs_per_day);
  int s_days = int (s_seconds / secs_per_day);

  b_seconds -= b_days * secs_per_day;
  s_seconds -= s_days * secs_per_day;

  return MJD (b_days+s_days, int(b_seconds+s_seconds), b_fracsec+s_fracsec);
}

MJD operator / (const Phase &p1, double frequency) {
  return p1 * (1.0/frequency);
}

Phase& Phase::operator ++ () {
  turns ++;
  settle();
  return *this;
}

Phase& Phase::operator -- () {
  turns --;
  settle();
  return *this;
}

Phase& Phase::operator += (int iturns) {
  turns += iturns;
  settle();
  return *this;
}

Phase& Phase::operator -= (int iturns) {
  turns -= iturns;
  settle();
  return *this;
}


// turns is converted to a Phase first, in order that large 'turns' does not
// destroy the precision of 'p1.fturns' -- WvS
Phase operator + (const Phase &p1, double turns) {
  return p1 + Phase(turns);
}

Phase operator - (const Phase &p1, double turns) {
  return p1 - Phase(turns);
}

Phase& Phase::operator += (double in_turns) {
  return *this = *this + Phase (in_turns);
}

Phase& Phase::operator -= (double in_turns) {
  return *this = *this - Phase (in_turns);
}

Phase& Phase::operator += (const Phase &p) {
  return *this = *this + p;
}

Phase& Phase::operator -= (const Phase &p) {
  return *this = *this - p;
}

static const double precision_limit = 2.0 * pow (10.0,-DBL_DIG);

int operator > (const Phase &p1, const Phase &p2)
{
  if (p1.turns != p2.turns)
    return p1.turns > p2.turns;

  // double precision_limit = 2*pow(10,-DBL_DIG);
  if (fabs (p1.fturns-p2.fturns) < precision_limit)
    return 0;
  else 
    return p1.fturns > p2.fturns;
}

int operator == (const Phase &p1, const Phase &p2) {
  if (p1.turns == p2.turns &&
      fabs (p1.fturns-p2.fturns) < precision_limit) 
    return 1;
  else
    return 0;  
}

int operator >= (const Phase &p1, const Phase &p2) {
  return p1>p2 || p1==p2;
}

int operator < (const Phase &p1, const Phase &p2) {
  return !(p1 >= p2);
}

int operator <= (const Phase &p1, const Phase &p2) {
  return p1<p2 || p1==p2;
}

int operator != (const Phase &p1, const Phase &p2) {
  return !(p1 == p2);
}

Phase Phase::Ceil ()
{
  return Phase (turns + (int64) ceil(fturns), 0.0);
}

Phase Phase::Floor ()
{
  return Phase (turns + (int64) floor(fturns), 0.0);
}

Phase Phase::Rint ()
{
  return Phase (turns + (int64) rint(fturns), 0.0);
}
