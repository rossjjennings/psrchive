
**Important Note:** The following instructions contain a few
intentional mistakes; the intent behind these is to demonstrate a
wider variety of related tools and problem solving strategies. If you
have limited time, first skim the instructions and then follow the
correct path.

# Measurement Equation Template Matching Example

Suppose that you have a well-calibrated template that spans some wide
band with some frequency resolution, and an observation that spans the
same band with the same frequency resolution. You can match the
observation to the template by running

```sh
pcm -H -S template.ar observation.ar
```

Here, `-H` is telling `pcm` to choose the number of
harmonics to include as constraints (the fit is performed in the
fluctuation frequency domain).
You might get an error message like

```console
Error::message
        mismatch between calibrator
        template.ar and
        observation.ar
        orientation mismatch: -135deg != 0deg
```

This is telling you that the one of receiver parameters is configured
differently in these files. To work around this, first have a look at the
receiver parameters; e.g.

```console
$ psredit template.ar | grep rcvr

rcvr:name   Receiver name                      MULTI
rcvr:basis  Basis of receptors                 lin
rcvr:hand   Hand of receptor basis             +1
rcvr:sa     Symmetry angle of receptor basis   -90deg
rcvr:rph    Reference source phase             0deg
rcvr:fdc    Receptor basis corrected           1
rcvr:prc    Receptor projection corrected      1
rcvr:ta     Tracking angle of feed             0deg

psredit observation.ar | grep rcvr

rcvr:name   Receiver name                      unknown
rcvr:basis  Basis of receptors                 lin
rcvr:hand   Hand of receptor basis             +1
rcvr:sa     Symmetry angle of receptor basis   45deg
rcvr:rph    Reference source phase             0deg
rcvr:fdc    Receptor basis corrected           0
rcvr:prc    Receptor projection corrected      0
rcvr:ta     Tracking angle of feed             0deg
```

Annoyingly, none of the parameters mention the word orientation; but you
notice that the difference in "Symmetry angle of receptor basis" is
equal to 135 degrees and conclude that this is the parameter to change; e.g.

```sh
psredit -c rcvr:sa=-90 observation.ar -m
```

Try again and if all goes well, you'll see messages like the following

```console
$ pcm -H -S template.ar observation.ar
pcm: loading archives
pcm: loaded archive: observation.ar
pcm: creating model
pcm: not normalizing Stokes parameters
pcm: loading and setting standard
pcm: standard set in 0.00286 seconds
pcm: adding observation
 using Receiver::tracking_angle=0 deg
 using Horizon::parallactic angle=85.7632044171018 deg
pcm: solving model
Calibration::SignalPath::solve failure 
        exceeded maximum number of iterations=50
Calibration::SignalPath::solve failure 
        exceeded maximum number of iterations=50
Calibration::SignalPath::solve failure 
        exceeded maximum number of iterations=50
Calibration::SignalPath::solve failure 
        exceeded maximum number of iterations=50
  reduced chisq 1336.17590332031
  reduced chisq 1100.93566894531
  reduced chisq 873.789123535156
  reduced chisq 605.220581054688
  reduced chisq 424.234100341797
  reduced chisq 299.593902587891
  reduced chisq 178.428039550781
  reduced chisq 109.12866973877
  reduced chisq 75.0532836914062
  reduced chisq 57.5628433227539
  reduced chisq 43.3002166748047
  reduced chisq 35.0598678588867
  reduced chisq 28.5796070098877
  reduced chisq 25.0979061126709
  reduced chisq 23.4912281036377
  reduced chisq 19.9330711364746
  reduced chisq 18.8966464996338
...
```

The output can be plotted with

```sh
psrplot -p calm pcm.fits
```

and queried using `psredit`; e.g. to list all polarization calibrator solution parameters

```console
$ psredit pcm.fits | grep pcal
pcal:name         Name of calibrator model                 van04e18
pcal:nparam       Number of calibrator model parameters    7
pcal:nchan        Number of frequency channels             128
pcal:mjd          Epoch of calibration observation         52886.9590000000
pcal:freq*        Centre frequency of each channel (MHz)   
pcal:wt*          Weight assigned to each channel          
pcal:eqn*:name*   Parameter name                           
pcal:eqn*:desc*   Parameter description                    
pcal:eqn*:val*    Parameter value                          
pcal:eqn*:var*    Parameter variance                       
pcal:eqn*:chisq   Chi squared                              
pcal:eqn*:nfree   Number of degrees of freedom
```

To see the names of the model parameters

```console
$ psredit -c 'pcal:eqn[64]:name' pcm.fits
pcm.fits pcal:eqn[64]:name=G,gamma,phi,el0,or0,el1,or1
```

Channel index 64 is not special, but you must choose a channel that has
a valid solution. At this point, you see that the model is parameterized
in terms of the ellipticities and orientations of each of the receptors,
when what you really want is a single parameter that describes the rotation
about the line of sight, as in Equation 19 of Britton (2000). So now you
rerun pcm with that model

```sh
pcm -H -S template.ar -m bri00e19 observation.ar
```

... wait a while and when pcm is finished

```console
$ psredit -c 'pcal:eqn[64]:name' pcm.fits
pcm.fits pcal:eqn[64]:name=G,gamma,phi,b_1,b_2,r_1,r_2
```

Now you have some boosts and rotations that have numerical indeces instead
of letters, but they appear in the same order, so r\_2 is r\_v, the rotation
about the line of sight. It's value in all frequency channels can be printed with

```sh
psredit -c 'pcal:eqn:val[6]' pcm.fits
```

but this spits out a long comma-separated list. You might want the values on separate lines in a single column, which can be achieved with

```sh
psredit -c 'pcal:eqn:val[6]' pcm.fits -d '\n'
```

and to get rid of the leading `pcm.fits pcal:eqn:val[6]=`

```sh
psredit -c 'pcal:eqn:val[6]' pcm.fits -d '\n' -Qq
```

this output can be redirected to a file named `radians.txt` and the
radio frequency of each channel can be queried and saved to file with

```sh
psredit -c 'pcal:freq' pcm.fits -d '\n' -Qq > frequencies.txt
```

Paste the two columns into a single file

```sh
paste frequencies.txt radians.txt > plot.txt
```

which can be plotted with gnuplot, where you can also fit a wavelength squared functional form to the curve to estimate the RM.
