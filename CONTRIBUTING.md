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

Start by creating a branch.
We give branches a name following `<type>/<subject>`.
Types are `feature` for feature development, `bugfix` for fixing bugs from the issues list, `hotfix` for small fixes without an issue, and `dev/<user>` for personal development branches that are not meant for merging.

This branch is immediately pushed, and a draft pull request is created for it.
The pull request can be created with the [`hub`](https://github.com/github/hub) tool: `hub pull-request -d`.
This draft pull request signals the others that someone is working on this feature/bugfix.
It allows others to see what you're doing before it is completed, and to give early feedback and suggestions.
For such a draft pull request, it is not yet necessary that the commits are nicely split up.


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
