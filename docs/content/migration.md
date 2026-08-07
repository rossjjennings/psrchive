# Migration to GitLab

Both psrchive and dspsr have migrated from SourceForge to GitLab.  
These notes are aimed at developers making the transition.

## Clone the Repository

It's easiest to start with a newly cloned repository:

```sh
git clone --recursive git@gitlab.com:pulsar-astronomy/psrchive.git
```

Everything is pretty much as it was before, with the following exceptions

* The default branch is now main (not master).

* The `More/MEAL` sub-directory and various supporting bits from `Util/units` have been extracted to a new project (pulsar-astronomy/meal) that is now included as a Git submodule at Util/meal.

## Modify the Repository

Changes can no longer be pushed directly to the main branch, and must be made on development branches.  To manage branches, It is best to externally document them using the following workflow.

1. Create a "New item" on the ["Work items" tab](https://gitlab.com/pulsar-astronomy/psrchive/-/work_items) of type “Issue”.

2. Describe in detail the new feature to be added or the new bug to be fixed.

3. Once the Work Item is created, click the "Create merge request" drop down and either
  * immediately create a merge request in the Draft state; or  
  * "Create branch"

4. Copy the branch name and, at the command line of a terminal, in the folder where the code has been checked out; e.g.,
```sh
git checkout -b new_branch_name  
```

5. Do all development on the branch and push changes to GitLab on this branch  
     
6. When ready, ask someone to review the merge request (MR) and, after it has been approved, click the "Merge" button.

## Migrate Development Branches from SourceForge

After step 4\. you can merge any changes that you pushed to SourceForge into your GitLab development branch as follows. 

```sh
git remote add sf ssh://***USER***@git.code.sf.net/p/psrchive/code  
git fetch sf -v  
git merge sf branch-name
```

## Update psrchive as a git submodule

If you manage a project that includes psrchive as a submodule, then you’ll need to update the submodule.  For example, if psrchive is included as a submodule in `resources/psrchive`, run the following commands.

```sh
git submodule set-url resources/psrchive https://gitlab.com/pulsar-astronomy/psrchive.git
cd resources/psrchive  
git pull origin main
```

Then commit the changes as usual for your project.

When others next do a git pull, they might encounter an error like the following:

```console
$ git pull  
...  
Fetching submodule resources/psrchive  
From https://git.code.sf.net/p/psrchive/code
   1ab5f512..3f3d5b43  master     -> origin/master  
fatal: remote error: upload-pack: not our ref 3b0608db3af789b7480a872bd338c50276b3e2e0  
Errors during submodule fetch:  
        resources/psrchive
```

To fix this, run the following commands

```sh
git submodule sync  
git submodule update --init --recursive  
git pull
```

If you’ve ever added an upstream remote in the submodule folder, then you’ll also need to update it; e.g.

```sh
cd resources/psrchive  
git remote remove upstream  
git remote add upstream git@gitlab.com:pulsar-astronomy/psrchive.git  
```