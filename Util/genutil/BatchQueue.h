//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* $Source: /cvsroot/psrchive/psrchive/Util/genutil/BatchQueue.h,v $
   $Revision: 1.1 $
   $Date: 2007/08/31 23:39:13 $
   $Author: straten $ */

#ifndef __BatchQueue_h
#define __BatchQueue_h

#include "ThreadContext.h"
#include "Functor.h"

#include <vector>

//! Runs multiple threads using a simple batch queue model
class BatchQueue {

public:

  //! Set the number of tasks that may run at one time
  void resize (unsigned nthread);

  //! A single job in the queue
  class Job;

  //! Submit a job for processing
  void submit (Job*);

  //! Submit a job for processing
  template<class Class, typename Method>
  void submit (Class* instance, Method method);

  //! Wait for completion of all active jobs
  void wait ();

protected:

  //! Add a job to the active list
  void add (Job*);

  //! Remove a job from the active list
  void remove (Job*);

  //! Mutual exclusion and condition variables used to coordinate the queue
  ThreadContext context;

  //! The active jobs
  std::vector<Job*> active;

};

class BatchQueue::Job {

public:

  virtual ~Job () {}

  //! Run the execute method and remove self from the active job list
  void run ();

  //! The job that is executed is defined by derived types
  virtual void execute () = 0;

protected:

  friend class BatchQueue;

  //! The batch queue in which this job is queued or running
  BatchQueue* queue;

};

template<class Class, typename Method>
void BatchQueue::submit (Class* instance, Method method)
{
  class Job0 : public Job {
    Functor< void() > functor;
  public:
    Job0 (Class* instance, Method method) : functor (instance, method) {}
    void execute () { functor (); }
  };
  
  submit (new Job0( instance, method ));
}


#endif // !defined(__BatchQueue_h)
