/***************************************************************************
 *
 *   Copyright (C) 2008 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "VirtualMemory.h"
#include "dirutil.h"

#include <vector>
#include <iostream>

using namespace std;

int main ()
{
  string filename = "test_VirtualMemory_swapspace";

  unsigned number_of_arrays = 100;
  unsigned array_size = 50;

  {
    // disable unlinking the swap file so that it can be tested
    bool unlink_swapfile = false;

    VirtualMemory manager (filename, unlink_swapfile);

    // the filename is used as the base for a random filename
    filename = manager.get_filename();

    cerr << "swap filename=" << filename << endl;

    float* null = 0;
    vector<float*> arrays (number_of_arrays, null);

    float count = 0;
    for (unsigned i=0; i < number_of_arrays; i++)
    {
      arrays[i] = manager.create<float> (array_size);
      for (unsigned j=0; j < array_size; j++)
      {
	arrays[i][j] = count;
	count ++;
      }
    }

    uint64_t size = filesize (filename.c_str());
    uint64_t expected = number_of_arrays * array_size * sizeof(float);
    if (size < expected)
    {
      cerr << "swap space size=" << size << " < expected=" << expected << endl;
      return -1;
    }

    for (unsigned i=0; i < arrays.size(); i++)
      manager.destroy (arrays[i]);
  }

  if (file_exists (filename.c_str()))
  {
    cerr << "swap file exists after VirtualMemory instance passed out of scope"
	 << endl;
    return -1;
  }

  cerr << "VirtualMemory class passes tests" << endl;

  return 0;
}
