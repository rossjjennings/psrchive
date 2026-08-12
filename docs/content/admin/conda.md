
# Release via conda-forge

1. Fork a copy of the [feedstock repo](https://github.com/conda-forge/psrchive-feedstock) to your own github area.

2. Make a new branch named something appropriate, and edit the `recipe/meta.yaml` file as follows:
  a. change the version in line 2 to whatever the latest one is.
  b. change the sha256sum in line 3 to match the current source tarball
  c. change the build number (currently line 21) to 0.

3. Commit these changes and push your branch to github.

4. Submit a PR to the main repo.

5. Write a comment on the PR containing "@conda-forge-admin, please rerender"

6. Wait for the automated build tests to succeed.

