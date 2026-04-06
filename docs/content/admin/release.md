# PSRCHIVE Administration Guide

## PSRCHIVE Administration Guide

### Releasing a Distribution

Upload the distribution file to the SourceForge File Release System (FRS)
by running the script

```
Management/release.csh
```

When it finishes, check that the release is posted at
[PSRCHIVE Files.

Update the following files:

```
web/changes/index.shtml
web/changes/release-<yyyy-mm-dd>.shtml
Documentation/psrchive.dox (PROJECT_NUMBER)
```

where "<yyyy-mm-dd>" is the release date.
When creating the release notes, it is useful to update the Bugs and
Feature Requests Trackers as follows:

* Visit [Update artifact groups for: Feature Requests](https://sourceforge.net/tracker/admin/index.php?group_id=159762&atid=813135&add_group=1) and scroll down to "Add a new artifact group" with the Group name "release <yyyy-mm-dd>"* Visit [Tracker: Feature Requests](https://sourceforge.net/tracker/?func=browse&group_id=159762&atid=813135&status=2&artgroup=804799) filtered on "Status: Closed" and "Group: next release" then "Check All" and do a "Mass Update" to set "Group: release <yyyy-mm-dd>". (It might also be a good idea to check for any stragglers in "Group: No Group".)* Change the filter to "Group: release <yyyy-mm-dd>" then click "Filter" then "Permalink" and cut-and-paste the URL into the release notes.* Visit [Update artifact groups for: Bugs](https://sourceforge.net/tracker/admin/index.php?group_id=159762&atid=813132&add_group=1) and scroll down to "Add a new artifact group" with the Group name "release <yyyy-mm-dd>"* Visit [Tracker: Bugs](https://sourceforge.net/tracker/?func=browse&group_id=159762&atid=813132&status=2&artgroup=804619) filtered on "Status: Closed" and "Group: next release" then "Check All" and do a "Mass Update" to set "Group: release <yyyy-mm-dd>". (Again, double-check "Group: No Group".)* Change the filter to "Group: release <yyyy-mm-dd>" then click "Filter" then "Permalink" and cut-and-paste the URL into the release notes.

Finally, [update the web pages](web.md).

Copyright © 2006-2019 Willem van Straten
Licensed under the Academic Free License version 2.1](https://sourceforge.net/projects/psrchive/files/psrchive/)
