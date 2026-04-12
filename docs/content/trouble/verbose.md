# PSRCHIVE User's Guide: Increasing Verbosity

By default, the error messages produced by PSRCHIVE
programs are quite terse. For example,

```
psredit -c name pulse_473145330.rf.it
TEMPO environment variable not defined
```

Why is TEMPO required to query an archive parameter?
Add the `-v` command line option to gain more information:

```
psredit -v -c name pulse_473145330.rf.it
psredit: error on pulse_473145330.rf.it
Error::stack
        Pulsar::Archive::load
        FITSArchive::load_header
        Pulsar::Telescopes::set_telescope_info
        Tempo::load_obsys
        Tempo::get_configuration
Error::InvalidState
Error::message
        TEMPO environment variable not defined
```

This sheds a little more light on the problem. At this point, you might
decide to either ensure that TEMPO is installed, so that observatory
information can be loaded from obsys.dat, or contact the
developers and suggest that this is an unreasonable constraint.

*Side Note:* This particular unreasonable constraint was removed
during the development of this documentation.

If you decide to seek help from the developers, please
send as much information as possible by enabling full verbosity mode
with the `-V` command line option; e.g.

```
psredit -c name -V pulse_473145330.rf.it >& psredit_bug.log
```

The log file can then be attached to the [Bug
Report](../support.md) (below the **Detailed Description**, at the **Upload
and Attach a File** prompt).
