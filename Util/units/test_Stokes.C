#include "Pauli.h"

int main ()
{
  Jones<float> jones;
  Stokes<float> in;

  Stokes<float> out = transform (in, jones);

  return 0;
}

