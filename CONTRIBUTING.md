# Contributing to prplMesh

You can contribute to prplMesh in various ways: reporting bugs, improving documentation, improving the code.

## Getting involved

The mailing list is very low traffic.
You can subscribe at [lists.prplfoundation.org](https://lists.prplfoundation.org/postorius/lists/prplmesh.lists.prplfoundation.org/) and [view the archives](https://lists.prplfoundation.org/hyperkitty/list/prplmesh@lists.prplfoundation.org/).

A large part of the conversation takes place directly on github, through issues, pull requests, and comments on commits.
Thus, it is advisable to subscribe to (i.e. watch) the issues you are interested in, or the project as a whole.

Daily conversations take place on [Slack](https://prplfoundation.slack.com/).
You can ask for an invite to anyone currently involved, e.g. by sending a message to the mailing list.

The [prpl Foundation Code of Conduct](https://prplfoundation.org/about/code-of-conduct/) applies to all communication about prplMesh.
If you encounter a problem with someone's conduct, contact the prpl Community Manager Mirko Lindner by emailing Mirko.Lindner [at] prplfoundation.org.


## Reporting bugs

To report bugs, use the [github issues](https:issues/) entry form.


## Improving documentation

High-level developer documentation can be found on the [wiki](https:wiki/), which can be edited in-place.

User documentation is still under development.
It currently resides as pptx and docx documents in the [documentation](documentation/) directory.
It is intended to be migrated to Jekyll markdown and to be published with github pages on https://prplfoundation.github.io/prplMesh.


## Contributing code

All development is managed using [pull requests](#pull-requests-workflow).
[Commits](#commits) must be in logical, consistent units and have a good commit message.
Every commit must carry a Signed-off-by tag to assert your agreement with the [DCO](#developers-certificate-of-origin).
Pull requests are reviewed and checks are performed on them before they are merged.
Code should adhere to the [coding style](#coding-style).

Note that everybody follows these same guidelines.
Core contributors, occasional contributors and maintainers are all alike.

### Commits

The code history is considered a very important aspect of the source code of the project.
It is part of the code's documentation, and allows anyone (including the original author) to find back why something was done in that particular way.
Therefore, we pay attention to how the commits look.

Every commit must stand on its own.
No commit should ever break something.
Commits should be split up into atomic units that perform one specific change.
E.g. to add a parameter to a function, there would be one commit to add the parameter and update all callers with the current value for it, and a second commit that changes one caller to use a different value for it.

The commit message is very important, it is part of the documentation.
It is relevant both during review of the contribution, and later to be able to understand why things are the way they are.
A commit message consists of a subject, a message body and a set of tags.

    component: class-or-function: change this and that

    The current situation is this and that. However, it should be that or
    that.

    We can do one or the other. One has this advantage, the other has this
    advantage. We choose one.

    Do this thing and do that thing.

    Signed-off-by: The Author <author.mail@address.com>
    Co-Authored-by: The Other Author <email@address.com>
    Signed-off-by: The Other Author <email@address.com>

The subject must be very short, it appears in the [short log](https:commits/master).
* Write it in the imperative: "add support for X".
* Start with a prefix that indicates the component: tlvf, common, documentation, bpl, bwl, bcl, bml, transport, topology, master, slave, monitor.
  Often, changes need to be made to multiple components, e.g. because of API change.
  The component that exports the changed API should be the one indicated then.
  If you feel multiple components apply, you should try to split up the commit into separate commits.
  The component does not always correspond to a directory.
  E.g. a change to [this file](CONTRIBUTING.md) would have `documentation` as component, even if it's not in the `documentation` directory.
* If the change is to a specific class or function, add it after the component
* In principle maximum 62 characters (including prefix).
  Because prefixes can be long (e.g. `transport: Ieee1905Transport::update_network_interfaces():`, this limit is often exceeded.
* Describes the "what" succinctly.
* Avoid verbs that don't say anything: "fix", "improve", "update", ...
* Be specific.
* Don't capitalize first word after the prefix.
* Don't write a full stop at the end of the subject line.

After the subject comes an empty line, followed by the extended description.
There is no limit to the length of the description.
It generally describes the "why" and "what" according to the following pattern:
* the context (current situation);
* the problem statement (why the current situation is not good);
* possible solutions with their pros and cons;
* the chosen solution and why it was chosen;
* the (core) change itself - in the imperative (i.e. "add foo()", not ("added foo()");
* additional changes needed to keep things working (e.g. "Update callers with the new argument").

Since commits should be split up, there will be many commits that are not useful on their own, but just prepare for another commit.
In that case, the preparatory commits should refer to what they prepare for.
For example, you could have the preparatory commit:

    tlvf: CmduMessageTx::finalize(): add swap_needed parameter

    Prepare for "tlvf: message_com::send_cmdu(): don't swap internal
    messages".

    Signed-off-by: Joe Developer <joe@prplfoundation.org>

Followed by the commit that actually makes the change:

    tlvf: message_com::send_cmdu(): don't swap internal messages

    Messages that are sent over UDS should not be swapped, since the
    receiver does not unswap them. Swapping is not needed anyway since the
    sender and receiver are guaranteed to be the same device over UDS.

    This can be detected using the dst_mac of the message, which is empty
    for messages that will be sent over UDS.

    Use the swap_needed parameter of CmduMessageTx::finalize() to avoid
    swapping.

    Signed-off-by: Joe Developer <joe@prplfoundation.org>

The extended description only needs to be added if there is actually something to say.
For example, the commit that added this document only had the subject line "documentation: add CONTRIBUTING.md" and no extended description.

The extended description should be wrapped at 72 columns (because `git log` indents with a tab, so it still fits on an 80-character line).
There are four exceptions:
* Strings found in log files should be copied as is, to simplify searching through the history.
* Similarly, for build failures, compilation output and error messages should be copied as is.
* URLs should never be split, for obvious reasons.
* The tags (Signed-off-by etc.) are never wrapped.

At the end of the commit message there is a block of tags.

The first tag must be a "Signed-off-by:" from the author of the commit.
This is a short way for you to say that you are entitled to contribute the patch under prplMesh's BSD+Patent license.
It is a legal statement that asserts your agreement with the [DCO](#developers-certificate-of-origin).
Therefore, it must have your *real name* (First Last) and a valid e-mail address.
Adding this tag can be done automatically by using `git commit -s`.
If you are editing files and committing through GitHub, you must write your real name in the “Name” field in GitHub settings and the email used in the "Signed-off-by:" must be your primary github email.
You must manually add the "Signed-off-by:" to the commit message that github requests.
If you are editing files and committing on your local PC, set your name and email with:
```shell
git config --global user.name "my name"
git config --global user.email "my@email.address"
```

### Pull Request workflow

This section describes the workflow followed by core contributors and maintainers.
It is advised to follow a similar workflow for your own contributions.
This section describes the full workflow.
For smaller contributions, you may take shortcuts.

The workflow is explained in detail below. In summary, it consists of these steps.
1. Create a branch
2. Create a draft pull request
3. Make the changes, commit with amend and rebase
4. Push regularly
5. Clean up the commits, push, and set pull request to "Ready for review"
6. Review is done.
7. Address review comments in additional fixup commits.
8. Review is approved by maintainers.
9. Use `git rebase -i --autosquash master` to clean up the pull request.
10. Pull request is merged by maintainers.

Start by creating a branch.
We give branches a name following `<type>/<subject>`.
Types are `feature` for feature development, `bugfix` for fixing bugs from the issues list, `hotfix` for small fixes without an issue, and `dev/<user>` for personal development branches that are not meant for merging.

This branch is immediately pushed, and a draft pull request is created for it.
The pull request can be created with the [`hub`](https://github.com/github/hub) tool: `hub pull-request -d`.
This draft pull request signals the others that someone is working on this feature/bugfix.
It allows others to see what you're doing before it is completed, and to give early feedback and suggestions.
For such a draft pull request, it is not yet necessary that the commits are nicely split up.

Continue developing the code.
Push your work regularly.
You can make separate commits, or amend a single commit, or rebase and sort it into different commits, at your option.
Every time you push, CI will run on the code and you will be informed of any issues with it.
Also, even if the pull request is still draft, maintainers may start giving comments on it.
The purpose of these comments is to make sure your work is aligned with expectations.
It avoids that after a lot of work, you are asked to still make major changes.

Once your feature is ready, use `git rebase -i` to organise it in clean commits and add a proper commit message to every commit, including a Signed-off-by.
Force-push the branch and change the pull request state from "Draft" to "Ready for review".

Other contributors will start reviewing your change and make suggestions for improvements.
The review has the following goals:
* Make sure maintainers stay aware of all the changes happening in the code.
* Identify opportunities for refactoring, code reuse, optimisation, ...
* Make sure the coding style and patterns stay consistent.
* Make sure the code remains maintainable and easy to understand.
* Brainstorm about better ways to write code or better architectural approaches.
* Identify potential bugs.
* Identify missing tests.

Very often, the review will lead to comments that don't really need to be addressed for the pull request to be accepted.
Many of the review goals are more about having a discussion than about really forcing changes.
Unfortunately, github doesn't have an easy way to make such a distinction, so reviewers have to mention explicitly when a suggestion is optional.
If there are review comments that need to be addressed, the pull request will be marked as "Changes requested".

If you make more changes after a pull request is marked as "Ready for review", do not rebase or amend.
This will allow reviewers to easily see the differences compared to their previous review.
Instead, create additional commits with `git commit --fixup <sha1 of commit to fix> -e`.
Do _not_ add a Signed-off-by to these commits.
This will block merging due to the DCO check, but that is intended because the fixup commits need to be applied first (see below).
In the fixup commit message, explain which review comments are addressed by the change - but don't change the subject line, since that is used by autosquash (see below).
If the commit message itself needs to be changed, use `git commit --squash <sha1 of commit to fix> -e`.
Below the subject line, add a second subject line, followed by the complete commit message and the Signed-off-by.
During the autosquash rebase, we'll only retain the second commit message (see below).

In the pull request's web interface, mark the comments that have been addressed as Resolved.
Also mark the comments that are optional as Resolved.
If you don't take the suggestion directly or you disagree or only apply it in part, don't mark it as Resolved.
Regularly push your branch.

There may be several iterations in the review.
Finally, the pull-request is marked as Approved by the maintainers.
However, it cannot be applied as is because the commits are still "dirty".
The last step is to perform `git rebase -i --autosquash master`.
This will apply the fixup commits automatically.
For the squash commits, it will stop in an editor to allow you to update the commit message.
Just use the last commit message, remove all the rest.
Finally, force-push the branch.
The DCO check will now succeed and the pull request is ready for merging.

Note that there are a few cases where it is not possible to use fixup commits.
In these cases, use rebase and create a clean series of commits again.
Add a comment in the pull request why this was done.
This is needed at least in the following cases:
* If you need to pull in changes from master or another branch or pull request.
* If you need to reorder the commits.
* If you need to move a subset of the changes from one commit to another commit.

If you are working on a big feature, you often encounter something small that needs to be fixed or improved that is relatively independent of the feature.
Such a change can be included as one of the first commits in the pull request.
However, often it's useful to create a separate pull request for it, so it can be applied more quickly.
The typical workflow for this is:
* Make the fix and commit it with a proper commit message.
* Check out a new branch `hotfix/<description>` based on master.
* Cherry-pick the fix commit.
* Push and create a pull request.
* Check out the development branch and rebase on the hotfix branch. This will automatically remove th fix commit.

### Definition of done

Before a pull request can be merged, it must be considered "Done".
That means the following conditions must hold.
* All commits have a Signed-off-by. Automatic with the DCO check.
* At least one maintainer has reviewed and approvied.
* Code builds on Ubuntu, with `MSGLIB=zmq` and `BWL_TYPE=DUMMY`. Automatic with Travis CI.
* Unit tests run successfully. Automatic with Travis CI.
* Controller and agent go to operational. Manual, by starting `local_bus`, `ieee1905_transport`, `beerocks_controller` and `beerocks_agent` and verifying that the agent log reports OPERATIONAL.
* Run clang-format.sh. If it fixes lines you did not change, commit that separately. TODO handle this in Travis CI.
* TODO valgrind / klockwork / clang-tidy / etc

### Coding style

TODO

<a id="developers-certificate-of-origin"></a>
## Developer's Certificate of Origin 1.1

By making a contribution to this project, I certify that:

* (a) The contribution was created in whole or in part by me and I
  have the right to submit it under the open source license
  indicated in the file; or

* (b) The contribution is based upon previous work that, to the best
  of my knowledge, is covered under an appropriate open source
  license and I have the right under that license to submit that
  work with modifications, whether created in whole or in part
  by me, under the same open source license (unless I am
  permitted to submit under a different license), as indicated
  in the file; or

* (c) The contribution was provided directly to me by some other
  person who certified (a), (b) or (c) and I have not modified
  it.

* (d) I understand and agree that this project and the contribution
  are public and that a record of the contribution (including all
  personal information I submit with it, including my sign-off) is
  maintained indefinitely and may be redistributed consistent with
  this project or the open source license(s) involved.
