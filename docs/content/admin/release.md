
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

2. Commit the change and skip the pipeline with
   ```sh
   git commit -m "release YYYY-MM-DD" .
   git push origin -o ci.skip
   ```

3. Create an annotated tag and launch the pipeline with
   ```sh
   git tag -a YYYY-MM-DD -m "release YYYY-MM-DD"
   git push origin --tags
   ````

The above commands result in the creation of two
[GitLab Pipelines](https://gitlab.com/pulsar-astronomy/psrchive/-/pipelines).
One has the final `release` job; the other is skipped.

The release pipeline verifies that the tag matches `configure.ac`, 
runs `make dist`, produces `psrchive-YYYY-MM-DD.tar.gz`, 
generates a CHANGELOG from the commits since the previous release tag,
and publishes the release as a set of downloadable assets at 
[GitLab Releases](https://gitlab.com/pulsar-astronomy/psrchive/-/releases).

