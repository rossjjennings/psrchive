
# Release a distribution

Releases are produced by the GitLab CI release pipeline, which is
triggered automatically when a date-format tag (`YYYY-MM-DD`) is
pushed.

The release maintainer's steps are:

1. On a clean checkout of the release branch, update
   `configure.ac` so that the `AC_INIT` version matches the
   intended tag, e.g.

   ```text
   AC_INIT([PSRCHIVE],[YYYY-MM-DD],[...])
   ```

2. Commit the change with
   ```sh
   git commit -m "release YYYY-MM-DD" .
   ```

3. Create an annotated tag with
   ```sh
   git tag -a YYYY-MM-DD -m "release YYYY-MM-DD"
   ````

4. Push both commits and the new tag with
   ```sh
   git push --follow-tags
   ```

5. The release pipeline verifies that the tag matches
   `configure.ac`, runs `make distcheck`, produces
   `psrchive-YYYY-MM-DD.tar.gz`, generates a CHANGELOG from the
   commits since the previous release tag, and publishes the
   GitLab Release.

The resulting Release page lists the tarball as a downloadable
asset. Nothing needs to be uploaded by hand.
