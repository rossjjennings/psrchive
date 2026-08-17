
# Release a distribution

Releases are produced by the GitLab CI release pipeline, which is
triggered automatically when a date-format tag (`YYYY-MM-DD`) is
pushed.

The release maintainer's steps are:

1. Create a release branch, update
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

3. Create a merge request for the release branch
   and stop the pipeline that is launched when
   the merge request is created.

4. Merge the branch into 'main' and stop the
   pipeline that is launched.

5. On the main branch, create an annotated tag and
   launch the pipeline with
   ```sh
   git tag -a YYYY-MM-DD -m "release YYYY-MM-DD"
   git push origin --tags
   ````

The last command results in the creation of a
[GitLab Pipeline](https://gitlab.com/pulsar-astronomy/psrchive/-/pipelines)
that has

* an initial `.pre` stage wiht a `validate-tag-version` jobs
* a final `release` stage with two jobs: `release` and `conda_forge`.

The `validate-tag-version` job verifies that the tag matches the version defined in `configure.ac`
and aborts the pipeline if necessary.

The `release` job takes the `psrchive-YYYY-MM-DD.tar.gz` file produced by the `distcheck` job
and publishes it as part of a new release at
[GitLab Releases](https://gitlab.com/pulsar-astronomy/psrchive/-/releases).

The `conda_forge` job creates a new release branch on a fork of the
[conda-forge psrchive-feedstock](https://github.com/conda-forge/psrchive-feedstock),
updates it to use the latest release, and submits a pull request.

