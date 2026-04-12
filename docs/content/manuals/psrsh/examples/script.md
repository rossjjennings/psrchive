# psrsh` examples

#### Shell script

The following script reduces the number of phase bins in each profile
to 512, performs scattered power correction, and weights each profile
by its signal-to-noise ratio:

```
#!/usr/bin/env psrsh

# reduce the number of phase bins to 512
bscrunch 512

# perform scattered power correction
spc

# weight each profile by its S/N
weight snr
```

If the above example were saved with the filename spc.psh,
and made executable (`chmod a+x spc.psh`), it could be used
to reduce a list of archives by running a command like the following:

```
spc.psh -m archive1 [archive2 ...]
```

Here, the `-m`  command line option is used to overwrite the
original data files.
