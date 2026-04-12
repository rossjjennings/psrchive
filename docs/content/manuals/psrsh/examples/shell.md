# psrsh` examples

#### Interactive shell

To load an archive, combine all frequency channels, and unload the
reduced archive with a new extension, the following sequence of
commands would be entered:

```
psrsh> load filename 
ok
psrsh> fscrunch
ok
psrsh> unload ext=newext
ok
```
