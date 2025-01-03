/***************************************************************************
 *
 *   Copyright (C) 2009 by Willem van Straten
 *   Licensed under the Academic Free License version 2.1
 *
 ***************************************************************************/

#include "fitsutil.h"
#include "Error.h"

#include <fitsio.h>
#include <math.h>

using namespace std;

void fits_version_check (bool verbose)
{
    float lib_version = 0.0;
    fits_get_version (&lib_version);

#if (CFITSIO_MAJOR < 4)
    // Older-style "X.YY" versioning
    float hdr_version = CFITSIO_VERSION;
#else
    // Newer "X.Y.Z" versioning.  fits_get_version now returns
    // X + 0.01*Y + 0.0001*Z according to:
    // https://heasarc.gsfc.nasa.gov/docs/software/fitsio/c/c_user/node52.html#ffvers
    float hdr_version = CFITSIO_MAJOR + 0.01*CFITSIO_MINOR + 0.0001*CFITSIO_MICRO;
#endif

    if (verbose)
        cerr << "fits_version_check:"
            " library=" << lib_version <<
            " header=" << hdr_version << endl;

    if ( fabs(lib_version - hdr_version) > 1e-4)
        throw Error (InvalidState, "fits_version_check",
                "CFITSIO version mismatch: version from libcfitsio library=%f does not equal version from fitsio.h header=%f",
                lib_version, hdr_version);
}
