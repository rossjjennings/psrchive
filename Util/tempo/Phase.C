#include <string.h>
#include <stdio.h>
#ifdef sun
#include <float.h>
#include <sunmath.h>
#endif
#include "Phase.h"


Phase::Phase(){
  turns = 0;
  fturns = 0;
}

Phase::Phase(int64 tns, double ftns) {
  turns = tns;
  turns += (int) ftns;
  fturns = ftns - (int) ftns;
  if(fturns<0 && turns>0) {
    fturns = 1.0+fturns;
    turns--;
  }
  if(fturns>0 && turns<0) {
    fturns = fturns-1.0;
    turns++;
  }
}

double Phase::in_turns() const {
  return(turns+fturns);
}

int64 Phase::intturns() const {
  return(turns);
}

double Phase::fracturns() const {
    return(fturns);
}

string Phase::strprint(int precision) const {

  if(precision>DBL_DIG){
    fprintf(stderr, "Phase::strprint warning: precision of %d exceeds that of a double",precision);
    fprintf(stderr, "- truncating to a precision of %d\n", DBL_DIG);
  }

  char ftn[30];
#ifdef sun
  sprintf(ftn, "%lld", this->intturns());
#else
  sprintf(ftn, "%ld", this->intturns());
#endif
  string s = ftn;
  sprintf(ftn, "%.*lf", precision, this->fracturns());
  if(this->fracturns()>=0) s += &(ftn[1]);
  else s += &(ftn[2]);
  return(s);
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

Phase operator + (const Phase &p1, double turns) {
  double addturns = p1.fturns + turns;
  return Phase(p1.turns,addturns);
}

Phase operator - (const Phase &p1, double turns) {
  double subturns = p1.fturns - turns;
  return Phase(p1.turns,subturns);
}

Phase& Phase::operator += (double in_turns) {
  Phase fze = *this + in_turns;
  *this = fze;
  return (*this);
}

Phase& Phase::operator -= (double in_turns) {
  Phase fze = *this - in_turns;
  *this = fze;
  return (*this);
}

int operator > (const Phase &p1, const Phase &p2) {
  if (p1.turns != p2.turns) return (p1.turns>p2.turns);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(p1.fturns-p2.fturns)<precision_limit) return(0);
  else return (p1.fturns>p2.fturns);
}

int operator >= (const Phase &p1, const Phase &p2) {
  if (p1.turns != p2.turns) return (p1.turns>p2.turns);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(p1.fturns-p2.fturns)<precision_limit) return(1);
  else return (p1.fturns>p2.fturns);
}

int operator < (const Phase &p1, const Phase &p2) {
  if (p1.turns != p2.turns) return (p1.turns<p2.turns);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(p1.fturns-p2.fturns)<precision_limit) return(0);
  else return (p1.fturns<p2.fturns);
}

int operator <= (const Phase &p1, const Phase &p2) {
  if (p1.turns != p2.turns) return (p1.turns<p2.turns);
  double precision_limit = 2*pow(10,-DBL_DIG);
  if(fabs(p1.fturns-p2.fturns)<precision_limit) return(1);
  else return (p1.fturns<p2.fturns);
}

int operator == (const Phase &p1, const Phase &p2) {
  double precision_limit = 2*pow(10,-DBL_DIG);
  if ((p1.turns == p2.turns) &&
      (fabs(p1.fturns-p2.fturns)<precision_limit)) 
      return (1);
  else
      return (0);  
}

int operator != (const Phase &p1, const Phase &p2) {
  double precision_limit = 2*pow(10,-DBL_DIG);
  if ((p1.turns != p2.turns) ||
      (fabs(p1.fturns-p2.fturns)>precision_limit)) 
      return (1);
  else
      return (0);  
}

