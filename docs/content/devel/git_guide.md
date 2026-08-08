# Git Overview

Developers with a GitLab account clone the PSRCHIVE repository with
```sh
git clone --recursive git@gitlab.com:pulsar-astronomy/psrchive.git
```

The `--recursive` flag ensures that all Git submodules are 
also checked out.  These can also be later checked out with
```sh
git submodule update --init --recursive
```

By default, the `main` branch is cloned:
```console
$ git status
On branch main
Your branch is up to date with 'origin/main'.
```

## Create a development branch

Each development task (e.g., a new *feature* or *bug fix*) must be
undertaken on a development branch that is later merged with main after code review.
This policy

1. keeps all work related to new features together and isolated from other work; and
2. allows simulataneous work on multiple tasks by switching between branches and merging when ready.

For example, create a new branch named 'psrapp'
```console
$ git checkout -b psrapp
Switched to a new branch 'psrapp'

$ git status
On branch psrapp
Your branch is up to date with 'origin/main'.
```

Add a new application
```console
$ cp More/Applications/example.C More/Applications/psrapp.C
$ git status
On branch psrapp
Untracked files:
   (use "git add <file> ..." to include in what will be committed)
     More/Applications/psrapp.C
nothing added to commit but untracked files present (use "git add" to track)
```

Add the code to the Git repo, and commit with a log message

```console
$ git add More/Applications/psrapp.C

$ git commit -m "An awesome new psrchive program that does nothing."
[psrapp 264901d] An awesome new psrchive program that does nothing.
 0 files changed, 0 insertions(+), 0 deletions(-)
 create mode 100644 More/Applications/psrapp.C

$ ls More/Applications/psrapp.C
More/Applications/psrapp.C

$ git branch
  main
* psrapp
```

Switch back to the *main* branch.

```console
$ git checkout main
Switched to branch 'main'
```

The psrapp.C no longer appears (because it is only on the *psrapp* branch).

```console
$ ls More/Applications/psrapp.C
ls: cannot access More/Applications/psrapp.C: No such file or directory
```

## Merge the development branch

To add the new *psrapp* program into the `main` branch, 

1. visit [New merge request](https://gitlab.com/pulsar-astronomy/psrchive/-/merge_requests/new)
1. fill out the Title and Description
1. add one or more reviewers
1. click "Create merge request"