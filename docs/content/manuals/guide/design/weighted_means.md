# User's Guide: Weighted Means

When supported by the underlying file format,
PSRCHIVE maintains and can assign a weight
to each frequency channel and integration interval. These weights
can be used to maximize signal-to-noise ratio, flag bad data, etc.

### Flagging Bad Data

Data can be manually or automatically flagged by
[paz](../../paz.md) or interactively flagged by
[pazi](../../pazi.md).

If a calibrator solution for any frequency channels cannot be found by
[pac](../../pac.md) or [pcm](../../pcm.md), then
these channels will be automatically flagged bad in the calibrator.
The corresponding channels will be flagged bad in any pulsar data to
which the calibrator is applied.

### Weighted Reference Frequency

Profile weights are applied whenever data are integrated in frequency
or time. Before computing the weighted means of the pulse profile
data, a weighted mean frequency will be assigned to them and, if
necessary, the data will be corrected for cold plasma dispersion and
Faraday rotation with respect to the weighted mean frequency.

In the remainder of this discussion, class attribute names correspond
to [psredit](../../psredit.md) parameter names as follows:

| `Class::get_attribute` | psredit parameter | Description |
| --- | --- | --- |
| Archive::get\_centre\_frequency | freq | Nominal centre frequency stored in header |
| Profile::get\_centre\_frequency | int:freq | Channel-specific centre frequency stored for each profile |
| Archive::get\_faraday\_corrected | rmc | Flag raised when `defaraday` method is called |
| Archive::get\_dedispersed | dmc | Flag raised when `dedisperse` method is called |

The following conventions apply:

1. Before calling defaraday (dedisperse), all profiles will have
   position angles (phases) relative to the centre frequencies of the
   individual frequency channels, as returned by
   Profile::get\_centre\_frequency, and Archive::get\_faraday\_corrected
   (Archive::get\_dedispersed) will return false.
2. After calling defaraday (dedisperse), all profiles will have
   position angles (phases) relative to the centre frequency of the whole
   band, as returned by Archive::get\_centre\_frequency, and
   Archive::get\_faraday\_corrected (Archive::get\_dedispersed) will return
   true.
3. The states of the two attributes, Archive::get\_faraday\_corrected
   and Archive::get\_dedispersed, are completely independent; for example,
   an archive may be dedispersed and still have Faraday rotated
   (i.e. uncorrected) position angles.

These conventions hold at all times; in particular, they are true both
before and after calling Archive::fscrunch and/or Archive::tscrunch.

Expressed in terms of the attribute names used by
[psredit](../../psredit.md):

* If dmc is false, then the phase of the profile in the
  ith sub-integration and the
  jth channel is associated with the radio
  frequency given by int[i]:freq[j].
* If dmc is true, then the phase of every profile is
  associated with the radio frequency given by freq.
* If rmc is false, then the position angle of the profile
  in the ith sub-integration and the
  jth channel is associated with the radio
  frequency given by int[i]:freq[j].
* If rmc is true, then the position angle of every profile is
  associated with the radio frequency given by freq.

Functionally, when integrating in time or frequency, if
Archive::get\_faraday\_corrected (Archive::get\_dedispersed) returns
false, then Faraday rotation (dispersion) will be corrected relative
to the weighted mean frequency of the resulting
channel/sub-integration before the profiles are added together. If
Archive::get\_faraday\_corrected (Archive::get\_dedispersed) returns
true, then all profiles will have already been corrected relative to
the centre frequency returned by Archive::get\_centre\_frequency.
