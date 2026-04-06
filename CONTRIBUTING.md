# Contributing to PSRCHIVE

## Submitting Changes

PSRCHIVE development has moved from SourceForge to GitLab. All new work
should be submitted via GitLab merge requests.

1. Fork or clone the repository from
   [gitlab.com/pulsar-astronomy/psrchive](https://gitlab.com/pulsar-astronomy/psrchive).
2. Create a feature branch from `master`.
3. Make your changes and push the branch to GitLab.
4. Open a merge request against `master`.

Direct pushes to `master` are restricted. All changes go through merge
request review.

## SourceForge Transition

During the transition period, a daily sync job pulls any new commits from
SourceForge `master` into GitLab `master`. This ensures that contributions
still landing on SourceForge are not lost.

**Contributors should stop pushing to SourceForge** and use GitLab merge
requests instead.

Once GitLab `master` moves ahead of SourceForge (because a merge request
has been merged), the sync job will detect the divergence and stop. At
that point SourceForge is fully deprecated for this repository.

### Sync Divergence

The sync uses `git merge --ff-only`, which fails cleanly if GitLab
`master` has diverged from SourceForge `master`. This is expected and
means:

- A merge request was merged into GitLab `master`, advancing it past
  SourceForge.
- The sync schedule should be disabled by a project maintainer.
- SourceForge is no longer the source of truth.

If the sync fails for any other reason (network issues, authentication),
the scheduled pipeline will show a failed `sync-sourceforge` job.
Maintainers should check the job log and resolve the issue.
