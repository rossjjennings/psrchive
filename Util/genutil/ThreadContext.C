/***************************************************************************
 *
 *   Copyright (C) 2007 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "ThreadContext.h"
#include "Error.h"
#include <errno.h>

ThreadContext::ThreadContext ()
{
#if HAVE_PTHREAD
  pthread_cond_init (&cond, NULL);
  pthread_mutex_init (&mutex, NULL);
#else
  // if pthreads aren't available, throw an exception
  throw Error (InvalidState, "ThreadContext ctor",
	       "pthread support is not available");
#endif
}

ThreadContext::~ThreadContext ()
{
}

void ThreadContext::lock ()
{
#if HAVE_PTHREAD
  errno = pthread_mutex_lock(&mutex);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_lock");
#endif
}

void ThreadContext::unlock ()
{
#if HAVE_PTHREAD
  errno = pthread_mutex_unlock(&mutex);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_unlock");
#endif
}

void ThreadContext::wait ()
{
#if HAVE_PTHREAD
  errno = pthread_cond_wait(&cond, &mutex);
  if (errno != 0) {
    pthread_mutex_unlock(&mutex);
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_wait");
  }
#endif
}

void ThreadContext::signal ()
{
#if HAVE_PTHREAD
  errno = pthread_cond_signal (&cond);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_signal");
#endif
}

void ThreadContext::broadcast ()
{
#if HAVE_PTHREAD
  errno = pthread_cond_broadcast (&cond);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_broadcast");
#endif
}

