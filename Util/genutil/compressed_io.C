#include <iostream>
#include <algorithm>

#include "machine_endian.h"
#include "compressed_io.h"

int fwrite_compressed (FILE* fptr, vector<float>& vals)
{
  vector<float>::iterator minel = min_element(vals.begin(), vals.end());
  vector<float>::iterator maxel = max_element(vals.begin(), vals.end());

  if (minel == vals.end() || maxel == vals.end()) {
    cerr << "fwrite_compressed: empty range" << endl;
    return -1;
  }
  float xmin = *minel;
  float xmax = *maxel;
  float ratio = 1.0;

  if (xmin == xmax) {
    fprintf (stderr, "fwrite_compressed: WARNING! no range in values!\n");
    fprintf (stderr, "fwrite_compressed: min == max == %e!\n", xmin);
  }
  else
    // dynamic range of quantization in this packing scheme is given by
    // the maximum value of an unsigned short or: (unsigned short)(~0x0)
    ratio = (xmax - xmin) / float((unsigned short)(~0x0));

  size_t writ = fwrite (&xmin, sizeof(float), 1, fptr);
  if (writ < 1) {
    perror ("fwrite_compressed: error fwrite(offset)\n");
    return -1;
  }
  writ = fwrite (&ratio, sizeof(float), 1, fptr);
  if (writ < 1) {
    perror ("fwrite_compressed: error fwrite(ratio)\n");
    return -1;
  }

  unsigned long size = vals.size();
  writ = fwrite (&size, sizeof(unsigned long), 1, fptr);
  if (writ < 1) {
    perror ("fwrite_compressed: error fwrite(size)\n");
    return -1;
  }

  unsigned short value = 0;
  for (vector<float>::iterator ind = vals.begin(); ind != vals.end(); ind++) {
    value = (unsigned short) ((*ind - xmin) / ratio);
    writ = fwrite (&value, sizeof(unsigned short), 1, fptr);
    if (writ < 1) {
      perror ("fwrite_compressed: error fwrite(value)\n");
      return -1;
    }
  }

  return 0;
}

int fread_compressed (FILE* fptr, vector<float>* vals, bool swapendian)
{
  size_t red;

  float offset = 0.0;
  red = fread (&offset, sizeof(float), 1, fptr);
  if (red < 1) {
    perror ("fread_compressed: error fread(offset)\n");
    return -1;
  }

  float ratio  = 1.0;
  red = fread (&ratio, sizeof(float), 1, fptr);
  if (red < 1) {
    perror ("fread_compressed: error fread(ratio)\n");
    return -1;
  }

  unsigned long size = 0;
  red = fread (&size, sizeof(unsigned long), 1, fptr);
  if (red < 1) {
    perror ("fread_compressed: error fread(size)\n");
    return -1;
  }

  if (swapendian) {
    ChangeEndian (offset);
    ChangeEndian (ratio);
    ChangeEndian (size);
  }

  vals->resize (size);

  unsigned short value = 0;
  for (vector<float>::iterator ind=vals->begin(); ind!=vals->end(); ind++) {
    red = fread (&value, sizeof(unsigned short), 1, fptr);
    if (red < 1) {
      perror ("fread_compressed: error fread(value)\n");
      return -1;
    }
    if (swapendian)
      ChangeEndian (value);

    *ind = offset + float(value) * ratio;
  }
  return 0;
}
