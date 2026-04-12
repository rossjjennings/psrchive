# CVS Access Instructions

### Anonymous CVS Access

The PSRCHIVE CVS repository can be accessed
through anonymous (pserver) CVS as follows. First, log in to the
CVS server with the following two commands:

```
touch $HOME/.cvspass
cvs -d:pserver:anonymous@psrchive.cvs.sourceforge.net:/cvsroot/psrchive login
```

When prompted for the password for *anonymous*, simply hit Enter.
Finally, download the software with the following command:

```
cvs -z3 -d:pserver:anonymous@psrchive.cvs.sourceforge.net:/cvsroot/psrchive co -P psrchive
```

After successfully downloading the code, follow the
[Build and Installation](cvs/build.md) directions.
