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
  pthread_cond_init (&cond, NULL);
  pthread_mutex_init (&mutex, NULL);
}

ThreadContext::~ThreadContext ()
{
}

void ThreadContext::lock ()
{
  errno = pthread_mutex_lock(&mutex);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_lock");
}

void ThreadContext::unlock ()
{
  errno = pthread_mutex_unlock(&mutex);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_unlock");
}

void ThreadContext::wait ()
{
  errno = pthread_cond_wait(&cond, &mutex);
  if (errno != 0) {
    pthread_mutex_unlock(&mutex);
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_wait");
  }
}

void ThreadContext::signal ()
{
  errno = pthread_cond_signal (&cond);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_signal");
}

