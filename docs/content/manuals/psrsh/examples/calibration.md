# psrsh` examples

## Polarimetric Calibration

Running the following script with the
`-e ext filenames`  command line options
is like running

```
pac -d database.txt -S -e ext filenames
```

```
#!/usr/bin/env psrsh

# Prefix a command with "init" so that it is run only once.
# This is useful when processing multiple files and 
# there is a real-time performance requirement.

# load list of calibrator files from database.txt
init cal load database.txt

# use the complete Reception model (like "pac -S")
init cal type ovhb04

# calibrate polarization using the nearest entry in database
cal
# calibrate flux using the nearest entry in database
cal flux
```
