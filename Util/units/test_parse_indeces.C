/***************************************************************************
 *
 *   Copyright (C) 2004 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/
#include "TextInterface.h"

using namespace std;
using namespace TextInterface;

static unsigned tests = 0;
static unsigned maxsize = 500;

void runtest (const string& input, const int* output)
{
  tests ++;

  unsigned size = 0;
  for (; size < maxsize; size++)
    if (output[size] == -1)
      break;

  if (size == maxsize) {
    cerr << "runtest: invalid input" << endl;
    exit (-1);
  }

  vector<unsigned> indeces;
  parse_indeces (indeces, input);

  if (indeces.size() != size) {
    cerr << "test " << tests << " input='" << input << "'"
      " size=" << indeces.size() << " != " << size << endl;
    exit(-1);
  }

  for (unsigned i=0; i<size; i++)
    if (indeces[i] != output[i]) {
      cerr << "test " << tests
           << ": index[" << i << "]=" << indeces[i] << " != " << output[i]
           << endl;
      exit(-1);
    }

}

int main ()
{
  try {
    int result[] = {3,-1};
    runtest ("[3]", result);
  }
  catch (Error& error) {
    cerr << "Caught unexpected exception: " << error.get_message() << endl;
    return -1;
  }

  try {
    int result[] = {3,5,-1};
    runtest ("[3,5]", result);
  }
  catch (Error& error) {
    cerr << "Caught unexpected exception: " << error.get_message() << endl;
    return -1;
  }

  try {
    int result[] = {3,4,5,6,-1};
    runtest ("[3-6]", result);
  }
  catch (Error& error) {
    cerr << "Caught unexpected exception: " << error.get_message() << endl;
    return -1;
  }

  try {
    int result[] = {0,8,9,10,11,3,4,5,6,-1};
    runtest ("[0,8-11,3-6]", result);
  }
  catch (Error& error) {
    cerr << "Caught unexpected exception: " << error.get_message() << endl;
    return -1;
  }

  try {
    int result[] = {-1};
    runtest ("variable", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1};
    runtest ("[0,8-11,-6]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1};
    runtest ("[0,,6]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1};
    runtest ("[6-]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1};
    runtest ("[6--8]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1};
    runtest ("[8-5]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1};
    runtest ("[]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1}; 
    runtest ("[-6-8]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  try {
    int result[] = {-1}; 
    runtest ("[1-4,6to8]", result);
    cerr << "Failed to throw exception on bad input" << endl;
    return -1;
  }
  catch (Error& error) {
    cerr << "Caught expected exception: " << error.get_message() << endl;
  }

  cerr << "parse_indeces passed all " << tests << " tests" << endl;

  return 0;
}

