# pcm

### Fit Report

Use the `-D report`  command line option to enable a report
of the reduced chisq for each Stokes parameter of each input state.
Each pulsar phase bin is a unique input state; the calibrator is
another input state. For each frequency channel, a text file named
pcm\_report\_*<ichan>*.txt will be output, where
*<ichan>* is the frequency channel index.

For each input state, a row will be printed; each row has six columns:
the input state index, the number of observations of that state, and
the reduced chisq of the four Stokes parameters (I,Q,U,V). The final
row reports the total number of independent measurements and the
average reduced chisq of each Stokes parameter over all states. For
example, the following report of the calibrator (state index 0) and
16 pulse phase bins (state indeces 1 to 16) indicates that Stokes U and
Stokes V (columns 5 and 6) of the calibrator are not well modeled.

```
0 12 3.68399 1.84899 1313.35 12650.2
1 14 1.75732 3.70238 6.02445 7.61791
2 14 1.07169 3.09063 4.59642 11.5015
3 14 0.931367 3.16005 7.84439 13.1984
4 14 1.86755 2.93617 7.77839 11.3878
5 14 0.928734 2.10617 4.47711 7.32862
6 14 1.48832 1.6981 2.68574 3.52452
7 14 1.1844 0.918844 2.40223 2.31119
8 14 0.754614 1.27211 1.77298 7.01515
9 14 0.870176 0.977137 1.90031 3.54046
10 14 0.962993 1.15924 2.77602 2.45146
11 14 0.918551 2.77822 2.21577 2.67845
12 14 1.55837 5.15421 2.96655 5.32287
13 14 1.22285 2.23092 7.4168 5.10944
14 14 0.750784 1.26479 4.13266 4.43651
15 14 1.38218 4.7973 5.19169 3.66642
16 14 1.43533 2.77311 4.09023 3.92493
TOTAL 236 1.3195 2.46805 70.8305 648.87
```

To help identify those observations with the greatest divergence from
the model, the fit reports will also list those observations with a
divergence (i.e. the square root of the contribution to the total
chisq) greater than 10.
The divergence is reported independently for each Stokes parameter,
listing the filename and sub-integration number of the observation.
In the following example, the calibrator Stokes V has high divergence
in sub-integrations 0 and 1 of s080921\_174911.cf and Stokes U
is highly divergent in the second chosen phase bin of
s080921\_182451.zrft.

```
divergence[3]=67.2243 s080921_174911.cf 0
divergence[3]=24.2092 s080921_174911.cf 1
[...]
0 12 3.68399 1.84899 1313.35 12650.2
1 14 1.1046 2.93993 7.15023 9.13914
divergence[2]=10.5858 s080921_182451.zrft 0
2 14 1.33686 3.89276 10.2331 9.69206
[...]
```
