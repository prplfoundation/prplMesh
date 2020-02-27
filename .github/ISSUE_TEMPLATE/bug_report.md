---
name: Bug report
about: Create a report to help us improve
title: "[BUG] <component>: <headline>"
labels: bug
assignees: ''
projects: prplfoundation/prplMesh/1
---

**Describe the bug** -
A clear and concise description of what the bug is. Always mention the tag / hash on which this was tested AND how often it was reproduced (for example - 1/10 tries).

## Steps to reproduce

Steps to reproduce the behavior:

1. Build
2. Run controller & agent
3. See in the logs this and that / process stops responding / crashing / system behaves unexpectedly

## Expected behavior

A clear and concise description of what you expected to happen.
For example - `bml_conn_map` should show 2 radios, but it showing none.

## Screenshots

If applicable, add screenshots to help explain your problem.

## Target

- Host: [e.g. Ubuntu 18.04]
- Platform: [e.g. Linux DUMMY mode with ZMQ]
- How you built prplMesh - full build command (for example `sudo ./build-image.sh && sudo ./build.sh -f MSGLIB=zmq CMAKE_BUILD_TYPE=Debug BUILD_TESTS=ON`
- How you ran prplMesh - for example `sudo build/install/scripts/prplmesh_utils.sh start`

## Additional context

Add any other context about the problem here.
