//-*-C++-*-
/***************************************************************************
 *
 *   Copyright (C) 2012 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

/* Util/genutil/DirectoryLock.h */

#ifndef __DirectoryLock_h
#define __DirectoryLock_h

#include <string>

//! Working directory in which only one process may operate at one time
class DirectoryLock
{

 public:

  //! Default constructor
  DirectoryLock (const char* path = 0);

  // set the directory in which system calls will be made
  void set_directory (const std::string&);
  // get the directory in which system calls will be made
  std::string get_directory () const;

  // get the name of the file used to lock the directory
  std::string get_lockfile () const;

  // lock the working directory
  void lock ();
  // unlock the working directory
  void unlock ();
  // clean the working directory
  void clean ();

protected:

  std::string path;
  std::string lockfile;

private:

  int lock_fd;
  bool have_lock;

  void open_lockfile ();

};

#endif

