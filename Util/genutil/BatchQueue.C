/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "BatchQueue.h"
#include "Error.h"

#include <errno.h>
#include <assert.h>

// #define _DEBUG 1
#include "debug.h"

using namespace std;

BatchQueue::BatchQueue (unsigned nthread)
{
#if HAVE_PTHREAD
  context = new ThreadContext;
  resize (nthread);
#else
  context = 0;
  if (nthread != 1)
    throw Error (InvalidState, "BatchQueue constructor",
		 "threads are not availalbe and nthread=%u", nthread);
#endif
}

BatchQueue::~BatchQueue ()
{
  DEBUG("BatchQueue::~BatchQueue this=" << this << " active.size=" << active.size());

#if HAVE_PTHREAD
  {
    ThreadContext::Lock lock (context);

    for (unsigned ithread = 0; ithread < active.size(); ithread++)
    {
      if ( active[ithread] )
      {
        DEBUG("BatchQueue dtor deleting active job=" << active[ithread]);
        delete active[ithread];
      }
    }
  }
#endif

  if (context)
    delete context;
}


#if HAVE_PTHREAD

void BatchQueue::resize (unsigned nthread)
{
  ThreadContext::Lock lock (context);

  unsigned jthread = 0;

  // move the currently active jobs to the front of the array
  for (unsigned ithread = 0; ithread < active.size(); ithread++)
    if ( active[ithread] != 0 ) {
      active[jthread] = active[ithread];
      jthread ++;
    }

  if ( nthread < jthread )
    throw Error (InvalidState, "BatchQueue::resize",
		 "cannot decrease nthreads to %u because there are %u active",
		 nthread, jthread);

  active.resize (nthread);

  for (unsigned ithread = jthread; ithread < nthread; ithread++)
    active[ithread] = 0;
}

static void* solve_thread (void* instance)
{
  BatchQueue::Job* thiz = static_cast<BatchQueue::Job*>(instance);

  DEBUG("BatchQueue::solve_thread this=" << thiz);

  thiz -> run ();

  // exit
  pthread_exit(NULL);
}

void BatchQueue::Job::run ()
{
  try {
    execute ();
  }
  catch (Error& e) {
    DEBUG(e.get_message());
  }

  queue->remove (this);
}

void BatchQueue::remove (Job* job)
{
  ThreadContext::Lock lock (context);

  DEBUG("BatchQueue::remove this=" << this << " job=" << job);

  unsigned ithread = 0;

  for (ithread = 0; ithread < active.size(); ithread++)
    if ( active[ithread] == job )
    {
      DEBUG("BatchQueue::remove deleting job=" << job);
      delete job;
      active[ithread] = 0;
      break;
    }

  assert ( ithread < active.size() );

  // signal completion to waiting scheduler
  context->signal();
}

void BatchQueue::add (Job* job)
{
  ThreadContext::Lock lock (context);

  unsigned ithread = 0;

  while ( active.size() ) {

    bool currently_solving = false;

    for (ithread = 0; ithread < active.size(); ithread++)
    {
      if ( active[ithread] == job )
        currently_solving = true;
    }

    if (currently_solving)
    {
      DEBUG("BatchQueue::solve_wait job=" << job << " is currently being solved");
    }
    else {

      for (ithread = 0; ithread < active.size(); ithread++)
      {
        if ( active[ithread] == 0 )
          break;
      }

      if ( ithread < active.size() )
        break;
    }

    DEBUG("BatchQueue::solve waiting for next available thread");

    context->wait ();
  }

  if ( !active.size() )
    throw Error (InvalidState, "BatchQueue::solve",
		 "cannot solve with zero available threads");

  active[ithread] = job;
}

//! Wait for all of the solutions to return
void BatchQueue::wait ()
{
  ThreadContext::Lock lock (context);
  
  while ( active.size() ) {

    unsigned current = 0;
    
    for (unsigned ithread = 0; ithread < active.size(); ithread++)
      if ( active[ithread] )
        current ++;
    
    if (!current)
      break;

    DEBUG("BatchQueue::wait waiting for " << current << " threads");

    context->wait ();

  }
}

void BatchQueue::submit (Job* job)
{
  DEBUG("BatchQueue::solve using " << active.size() << " threads");

  if (active.size() == 0)
  {
    DEBUG("BatchQueue::solve calling Job::execute");
    job->execute();
    delete job;
    return;
  }

  add (job);

  job->queue = this;

  pthread_attr_t pat;
  pthread_attr_init (&pat);
  pthread_attr_setdetachstate (&pat, PTHREAD_CREATE_DETACHED);
  pthread_t thread_ID;

  DEBUG("BatchQueue::solve creating solve thread job=" << job);

  errno = pthread_create (&thread_ID, &pat, solve_thread, job);

  if (errno != 0)
    throw Error (FailedSys, "BatchQueue::solve", "pthread_create");
}

#else // ! HAVE_PTHREAD

//! Set the number of instances that may be solved simultaneously
void BatchQueue::submit (Job* job)
{
  job->execute();
}

void BatchQueue::wait ()
{
}

void BatchQueue::resize (unsigned nthread)
{                 
  if (nthread > 0)
    throw Error (InvalidParam, "BatchQueue::resize", "threads unavailable");
}

#endif

