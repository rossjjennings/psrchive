#! /usr/bin/env python

# make_fake_archive.py
#
# Example showing how to make a fake (simulated data) Archive
# from scratch using python.  For convenience uses the parfile
# parsing class from Presto.
#
# P. Demorest 2013/03

import numpy
import psrchive
import parfile

# Dimensions
nsub = 10
npol = 4
nchan = 128
nbin = 512

# Times
start_time = psrchive.MJD(56000,12345,0.0) # days, seconds, fractional sec
tsub = 30.0 # Time of each subint in seconds

# Freqs, MHz
freq = 1500.0
bw = 800.0
chan_freqs = bw * (numpy.arange(float(nchan))/nchan - 0.5) + freq

# Relevant filenames
parfile_name = 'fake.par'
outputfile_name = 'fake.fits'

# Create the Archive instance.
# This is kind of a weird hack, if we create a PSRFITS
# Archive directly, some header info does not get filled
# in.  If we instead create an ASP archive, it will automatically
# be (correctly) converted to PSRFITS when it is unloaded...
arch = psrchive.Archive_new_Archive("ASP")
arch.resize(nsub,npol,nchan,nbin)

# Get/set some parameters from the par file.  If you don't have
# Presto parfile package installed, can replace this with hard-coded 
# (or whatever) values.
par = parfile.psr_par(parfile_name)
arch.set_source(par.PSR)
arch.set_dispersion_measure(par.DM)
# Dec needs to have a sign for the following sky_coord call
if (par.DECJ[0] != '+' and par.DECJ[0] != '-'):
    par.DECJ = "+" + par.DECJ
arch.set_coordinates(psrchive.sky_coord(par.RAJ + par.DECJ))

# Set some other stuff
arch.set_centre_frequency(freq)
arch.set_bandwidth(bw)
arch.set_telescope('1')
if npol==4:
    arch.set_state('Coherence') # Could also do 'Stokes' here

# Fill in some integration attributes
epoch = start_time
epoch += tsub/2.0
for subint in arch:
    subint.set_epoch(epoch)
    subint.set_duration(tsub)
    epoch += tsub
    for ichan in range(nchan):
        subint.set_centre_frequency(ichan,chan_freqs[ichan])

# Fill in polycos
arch.set_ephemeris(parfile_name)

# Now finally, fill in the data!
for subint in arch:
    for ipol in range(npol):
        for ichan in range(nchan):
            prof = subint.get_Profile(ipol,ichan)
            prof.get_amps()[:] = numpy.random.randn(nbin)

# Unload the result
arch.unload(outputfile_name)
