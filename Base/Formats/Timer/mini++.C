#include <stdio.h>
#include <math.h>

#include "mini++.h"
#include "convert_endian.h"

string Mini::reason;

void Mini::init (struct mini& hdr)
{
  hdr.junk = hdr.junk2 = hdr.junk3 = -1;

  hdr.mjd = 0;
  hdr.fracmjd = 0.0;
  hdr.lst_start = 0.0;
  hdr.pfold = 0.0;
  hdr.tel_zen = hdr.tel_az =0.0;
  hdr.feed_ang = hdr.para_angle = 0.0;
  hdr.version = 0.0;
  hdr.pulse_phase = 0.0;
  hdr.flux_A = hdr.flux_B = 0.0;
  hdr.integration = 0.0;
  hdr.space[0] = '\0';
}

int Mini::fload (const char* fname, struct mini* hdr, bool big_endian)
{
  FILE* fptr = fopen (fname, "r");
  if (fptr == NULL) {
    fprintf (stderr, "Mini::load Cannot open '%s'", fname);
    perror ("");
    return -1;
  }
  int ret = load (fptr, hdr, big_endian);
  fclose (fptr);
  return ret;
}

int Mini::load (FILE* fptr, struct mini* hdr, bool big_endian)
{
  if (fread (hdr, sizeof(struct mini), 1, fptr) < 1)  {
    fprintf (stderr, "Mini::load Cannot read mini struct from FILE*");
    perror ("");
    return -1;
  }
  if (big_endian)
    mini_fromBigEndian (hdr);
  else
    mini_fromLittleEndian (hdr);
  return 0;
}

// unloads a mini struct to a file (always big endian)
int Mini::unload (FILE* fptr, struct mini& hdr)
{
  mini_toBigEndian (&hdr);
  if (fwrite (&hdr, sizeof(struct mini), 1, fptr) < 1)  {
    fprintf(stderr,"Mini::unload Cannot write mini struct to FILE*");
    perror ("");
    return -1;
  }
  mini_fromBigEndian(&hdr);
  return 0;
}

MJD Mini::get_MJD (const struct mini& hdr)
{ 
  return MJD(hdr.mjd, hdr.fracmjd);
}

void Mini::set_MJD (struct mini& hdr, const MJD& mjd)
{
  hdr.mjd = mjd.intday();
  hdr.fracmjd = mjd.fracday();
}
