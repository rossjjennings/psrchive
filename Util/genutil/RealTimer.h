/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/RealTimer.h,v $
   $Revision: 1.1 $
   $Date: 2003/05/15 09:23:29 $
   $Author: hknight $ */

#ifndef __RealTimer_H
#define __RealTimer_H

#include <iostream>
#include <sys/time.h>

#include "Reference.h"
#include "psr_cpp.h"

class RealTimer : public Reference::Able{
 public:
  RealTimer();
  ~RealTimer();

  //! start the clock
  void start();

  //! stop the clock; reset elapsed time and add to total time
  void stop();

  //! same as stop
  void lap();

  //! reset the start total and elapsed values
  void reset();

  //! return the seconds of elapsed real-time between start and stop
  double get_elapsed() const;

  //! return the cummulative sum
  double get_total() const;

  const char* elapsedString ();

  static const char* timeString (double time);

 private:
  struct timeval time1, time2;
  
  double elapsed;         // the time elapsed between start() and stop() calls
  double total_elapsed;   // cummulative sum over the life of this instance
};

ostream& operator<< (ostream& ostr, const RealTimer& clock);

#endif

