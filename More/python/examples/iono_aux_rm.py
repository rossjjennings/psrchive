#!/usr/bin/env python
# coding: utf-8
# 
# ***************************************************************************
#
#   Copyright (C) 2025 by Maaijke Mevius and Willem van Straten
#   Licensed under the Academic Free License version 2.1
#
# ***************************************************************************

import astropy.units as u
import numpy as np
import os
import psrchive as psr
from astropy.coordinates import EarthLocation, SkyCoord
from astropy.time import Time
from spinifex import get_rm

"""Set the aux:rm in each sub-integration to the ionospheric RM estimated using spinifex."""

def update_aux_rm(filenames, new_extension):

    times = []

    for file in filenames:
        print(f"loading information from {file=}")
        ar = psr.Archive.load(file)
        artimes = ar.get_mjds()
        arloc = ar.get_ant_xyz()
        arcoord = ar.get_coordinates()

        if not times:
            telescope_location = arloc
            coord = arcoord
            reference_file = file
        else:
            # Ensure that the observatory has not moved between files
            if arloc != telescope_location:
                print(f"observatory location in {file=} does not equal location in file={reference_file}")
                exit()

            # Ensure that the pulsar has not moved between files
            if not arcoord.equals(coord):
                print(f"source coordinates {arcoord.getHMSDMS()} in {file=} \n"
                      f"do not equal coordinates {coord.getHMSDMS()} in file={reference_file}")
                exit()

        if isinstance(artimes, list):
            times.extend(artimes)
        else:
            times.append(artimes)

    if telescope_location == 'undefined':
        print(f"telescope undefined - assuming Parkes")
        telescope_location = [-4554231.5, 2816759.1, -3454036.3]

    loc = EarthLocation.from_geocentric(*telescope_location, unit=u.m)

    # create source object
    ra=coord.ra().getRadians() * u.rad
    dec=coord.dec().getRadians() * u.rad
    source = SkyCoord(ra=ra, dec=dec)

    times = Time(np.array(times).flatten(), format='mjd')
    iono_kwargs = {}
    iono_kwargs['prefix'] = 'uqr'
    iono_kwargs['server'] = 'chapman'

    rm = get_rm.get_rm_from_skycoord(loc=loc, times=times, source=source, iono_kwargs=iono_kwargs)
    # print to screen
    rotation_measures = rm.rm
    rm_times = rm.times
    elevations = rm.elevation
    azimuths = rm.azimuth
    print("time      RM (rad/lambda^2)      azimuth (deg)      elevation (deg)")
    for myrm, tm, az, el in zip(rotation_measures, rm_times, azimuths, elevations):
        print(f"{tm.isot} {myrm} {az:3.2f} {el:2.2f}")

    idx = 0

    for file in filenames:
        print(f"setting aux:rm in {file=}")
        ar = psr.as_psrfits( psr.Archive.load(file) )

        nsubint = ar.get_nsubint()
        for isubint in range(nsubint):
            subint = ar.get_Integration(isubint)
            aux = subint.add_aux_cold_plasma_measures()
            aux.set_rotation_measure(rotation_measures[idx])
            idx += 1

        base, ext = os.path.splitext(file)
        new_filename = base + "." + new_extension
        print(f"unloading {new_filename}")
        ar.unload(new_filename)

def main() -> None:
    """Set the aux:rm in each sub-integration to the ionospheric RM estimated using spinifex."""
    import argparse

    # do arg parsing here
    p = argparse.ArgumentParser()
    p.add_argument(
        "--itrf",
        type=str,
        help="ITRF coordinates of observatory (not impelemented)",
    )

    p.add_argument(
        "--ext",
        type=str,
        default="irm",
        help="output filename extension",
    )

    args, files = p.parse_known_args()
    update_aux_rm(files, args.ext)

if __name__ == "__main__":
  main()