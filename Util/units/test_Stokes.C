#include "Stokes.h"

int main ()
{
  Jones<float> jones;
  Stokes<float> in;

  in = jones;

  Stokes<float> out = jones * in * herm(jones);

  return 0;
}

