# Git: Quick Guide (for Developers)

Clone the PSRCHIVE Git repo using your sf.net username and password.

```
$ git clone ssh://USERNAME@git.code.sf.net/p/psrchive/code psrchive
```

Each development task (e.g. new *feature* or *bug fix* should be
performed on a branch—this will naturally separate the work. So, if you are
writing a PSRCHIVE application **and** you are also
modifying the Pulsar::Archive class for fun, these two (independent) tasks can be
performed in a single Git repo *simultaneously* by switching branches.

```
$ git status
# On branch master
nothing to commit (working directory clean)
```

By default, the could would have been pulled the master (default) branch from
the Git repo. Let's proceed and create the new psrchive application: 'psrapp'.

```
$ git checkout -b psrapp
Switched to a new branch 'psrapp'

$ git status
# On branch psrapp
nothing to commit (working directory clean)
```

Create the new application.

```
$ touch More/Applications/psrapp.C

$ git status
# On branch psrapp
# Untracked files:
#   (use "git add ..." to include in what will be committed)
#
#     More/Applications/psrapp.C
nothing added to commit but untracked files present (use "git add" to track)
```

Add the code to the Git repo.

```
$ git add More/Applications/psrapp.C
```

Commit the file.

```
$ git commit -m "An awesome new psrchive program that does nothing."
[psrapp 264901d] An awesome new psrchive program that does nothing.
 0 files changed, 0 insertions(+), 0 deletions(-)
 create mode 100644 More/Applications/psrapp.C

$ ls More/Applications/psrapp.C
More/Applications/psrapp.C

$ git branch
  master
* psrapp
```

Switch back to the *master* branch.

```
$ git checkout master
Switched to branch 'master'
```

The psrapp.C is now longer present (as it is on the *psrapp* branch).

```
$ ls More/Applications/psrapp.C
ls: cannot access More/Applications/psrapp.C: No such file or directory
```

By now, you should get the idea that developing each new feature or bug fix
should be performed on separate branches. This will:

1. Keep all the work related to the new feature together (isolated from other work)
2. Allow simulataneous work on multiple tasks at the same time (by switching between branches and merging when ready)

In order to get our new *psrapp* program into the *master* branch so
it can be committed to the central Git repo, we need to perform a merge:

```
$ git merge psrapp
Updating 913a8e7..264901d
Fast-forward
 0 files changed, 0 insertions(+), 0 deletions(-)
 create mode 100644 More/Applications/psrapp.C

$ ls More/Applications/psrapp.C
More/Applications/psrapp.C
```

Now, we have our new psrchive program in the master branch. When it is ready for
other developers to access, push it into the central Git repo.
**Don't actually do this command if you're following the walk-through.**

```
$ git push origin master
```
