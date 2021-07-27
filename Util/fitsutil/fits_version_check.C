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
    float version = 0.0;
    fits_get_version (&version);

    float lib_version = CFITSIO_MAJOR + CFITSIO_MINOR/pow(10,ceil(log10(CFITSIO_MINOR)));

    if (verbose)
        cerr << "fits_version_check:"
            " library=" << version <<
            " header=" << lib_version << endl;

    if ( fabs(version - lib_version) > 1e-4)
        throw Error (InvalidState, "fits_version_check",
                "CFITSIO version mismatch: library=%f header=%f",
                version, lib_version);
}
