/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Calibration/ReceptionModel.h"

#include <errno.h>

using namespace std;

//! work around stupid compiler
static Calibration::ReceptionModel* null = 0;

//! By default, only one thread is used
static vector<Calibration::ReceptionModel*> current_solve (1, null);

#if HAVE_PTHREAD

#include <pthread.h>

//! condition by which exiting threads signal that they have finished
static pthread_cond_t  __lcond = PTHREAD_COND_INITIALIZER;

//! the mutex that guards __lcond
static pthread_mutex_t __lmutex = PTHREAD_MUTEX_INITIALIZER;

static void __lock_mutex ()
{
  errno = pthread_mutex_lock(&__lmutex);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_lock");
}

static void __unlock_mutex ()
{
  errno = pthread_mutex_unlock(&__lmutex);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_mutex_unlock");
}

static void __cond_wait ()
{
  errno = pthread_cond_wait(&__lcond, &__lmutex);
  if (errno != 0) {
    pthread_mutex_unlock(&__lmutex);
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_wait");
  }
}

static void __cond_signal ()
{
  errno = pthread_cond_signal (&__lcond);
  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_cond_signal");
}

//! Set the number of instances that may be solved simultaneously
void Calibration::ReceptionModel::set_nsolve (unsigned nthread)
{
  __lock_mutex ();

  unsigned jthread = 0;

  for (unsigned ithread = 0; ithread < current_solve.size(); ithread++)
    if ( current_solve[ithread] != 0 ) {
      current_solve[jthread] = current_solve[ithread];
      jthread ++;
    }

  if ( nthread < jthread ) {
    cerr << "Calibration::ReceptionModel::set_nsolve already " << jthread
	 << " solutions" << endl;

    __unlock_mutex ();

    return;
  }

  current_solve.resize (nthread);

  for (unsigned ithread = jthread; ithread < nthread; ithread++)
    current_solve[ithread] = 0;

  __unlock_mutex ();

}

void* Calibration::ReceptionModel::solve_thread (void* instance)
{
  ReceptionModel* thiz = static_cast<ReceptionModel*>(instance);

  if (verbose)
    cerr << "Calibration::ReceptionModel::solve_thread this=" << thiz << endl;

  try { 
    // call the solve_work method
    thiz -> solve_work ();
  }
  catch (Error& error)  {
    cerr << "Calibration::ReceptionModel::solve_thread error" 
         << error << endl;
  }

  __lock_mutex ();

  // remove self from list of pending solutions
  unsigned ithread = 0;

  for (ithread = 0; ithread < current_solve.size(); ithread++)
    if ( current_solve[ithread] == thiz ) {
      current_solve[ithread] = 0;
      break;
    }

  __unlock_mutex ();

  if ( ithread == current_solve.size() )  {
    cerr << "Calibration::ReceptionModel::solve_thread internal error" << endl;
    abort();
  }
 
  // signal completion to waiting solve method
  __cond_signal ();

  // exit
  pthread_exit(NULL);
}

//! Set the number of instances that may be solved simultaneously
void Calibration::ReceptionModel::solve_wait (ReceptionModel* to_solve)
{
  __lock_mutex ();
  
  unsigned ithread = 0;

  while ( current_solve.size() ) {

    bool currently_solving = false;

    for (ithread = 0; ithread < current_solve.size(); ithread++)
      if ( current_solve[ithread] == to_solve )
	currently_solving = true;
    
    if (currently_solving)
      cerr << "Calibration::ReceptionModel::solve_wait to_solve=" << to_solve
	   << " is currently being solved" << endl;

    else {

      for (ithread = 0; ithread < current_solve.size(); ithread++)
	if ( current_solve[ithread] == 0 )
	  break;

      if ( ithread < current_solve.size() )
	break;

    }

    if (verbose) cerr << "Calibration::ReceptionModel::solve"
                         " waiting for next available thread" << endl;

    __cond_wait ();

  }

  if ( !current_solve.size() )
    throw Error (InvalidState, "Calibration::ReceptionModel::solve",
		 "cannot solve with zero available threads");

  current_solve[ithread] = to_solve;

  __unlock_mutex ();

}

//! Wait for all of the solutions to return
void Calibration::ReceptionModel::solve_wait ()
{
  __lock_mutex ();
  
  while ( current_solve.size() ) {

    unsigned currently_solving = 0;
    
    for (unsigned ithread = 0; ithread < current_solve.size(); ithread++)
      if ( current_solve[ithread] )
	currently_solving ++;
    
    if (currently_solving)
      cerr << "Calibration::ReceptionModel::solve_wait waiting for "
	   << currently_solving << " threads" << endl;

    else
      break;
    
    __cond_wait ();

  }

  __unlock_mutex ();
}

void Calibration::ReceptionModel::solve ()
{
  if (verbose)
    cerr << "Calibration::ReceptionModel::solve using " << current_solve.size()
         << " threads" << endl;

  solve_wait (this);

  pthread_attr_t pat;
  pthread_attr_init (&pat);
  pthread_attr_setdetachstate (&pat, PTHREAD_CREATE_DETACHED);
  pthread_t thread_ID;

  if (verbose)
    cerr << "Calibration::ReceptionModel::solve creating solve thread"
            " this=" << this << endl;

  errno = pthread_create (&thread_ID, &pat, solve_thread, this);

  if (errno != 0)
    throw Error (FailedSys, "Calibration::ReceptionModel::solve",
		 "pthread_create");
}

#else // ! HAVE_PTHREAD

//! Set the number of instances that may be solved simultaneously
void Calibration::ReceptionModel::solve_wait (ReceptionModel* to_solve)
{
}

void Calibration::ReceptionModel::solve_wait ()
{
}

void Calibration::ReceptionModel::solve ()
{
  solve_work ();
}

void Calibration::ReceptionModel::set_nsolve (unsigned nthread)
{                 
  if (nthread > 1)
    cerr << "Calibration::ReceptionModel::set_nsolve threads unavailable"
         << endl;
}

#endif

