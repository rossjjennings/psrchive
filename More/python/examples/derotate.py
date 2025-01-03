#! /usr/bin/env python

# Corrects Faraday rotation to infinite frequency

import psrchive

from optparse import OptionParser
usage = """Usage: %prog [options] file (file2 ...)

Corrects Faraday rotation to infinite frequency
for each PSRCHIVE file."""

par = OptionParser(usage=usage)
par.add_option("-e", "--ext", dest="ext", help="Output file extension (.R)",
        action="store", type="string", default="R")
(opt,args) = par.parse_args()

for f in args:
    try:
        ar = psrchive.Archive_load(f)
        outf = f + "." + opt.ext

        rotate = psrchive.FaradayRotation()
        rotate.set_reference_wavelength(0.0)
        rotate.execute(ar)

        print(f"Writing '{outf}' ...")
        # rotate = psrchive.FaradayRotation
        ar.unload(outf)
    except Exception as exception:
        print(f"Error processing {f}")
        print(exception)

