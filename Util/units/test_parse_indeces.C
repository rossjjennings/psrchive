#include "TextInterface.h"

using namespace std;
using namespace TextInterface;

int main ()
{
  vector<unsigned> indeces;

  string name = "[3]:";
  parse_indeces (indeces, name);
  if (name != "") {
    cerr << "test 1: name not empty" << endl;
    return -1;
  }
  if (indeces.size() != 1) {
    cerr << "test 1: range size != 1" << endl;
    return -1;
  }
  if (indeces[0] != 3) {
    cerr << "test 1: first element != 3" << endl;
    return 0;
  }

  return 0;
}
