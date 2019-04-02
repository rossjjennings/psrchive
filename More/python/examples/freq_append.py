#! /usr/bin/env python

# freq_append.py - P. Demorest 2019/04
#
# Simple example of how to use the psrchive FrequencyAppend class in python.

import sys
import psrchive

# Load all the archives specified on the command line
archives = []
for fname in sys.argv[1:]:
    try:
        archives.append(psrchive.Archive_load(fname))
    except:
        print "Error loading archive '%s'" % fname

# Set up append classes
freqappend = psrchive.FrequencyAppend()
patch = psrchive.PatchTime()
# This is needed for single-pulse data:
# patch.set_contemporaneity_policy("phase")

# Explicitly align all archives to same polycos, and append them to archives[0]
# FrequencyAppend may also be able to re-align internally/automatically
freqappend.init(archives[0])
freqappend.ignore_phase = True
polycos = archives[0].get_model()
for arch in archives[1:]:
    arch.set_model(polycos)
    patch.operate(archives[0],arch)
    freqappend.append(archives[0],arch)

# unload it to combined.ar
archives[0].unload('combined.ar')
