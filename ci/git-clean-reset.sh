#!/bin/sh -e

# Reset the repository pointed by $1 to the revision given by $2

cd "$1" || {
    echo "Failed to cd into \"$1 \". Skipping reset."
    exit 1
}

git fetch --recurse-submodules
git clean -fd
git submodule foreach --recursive git clean -fd
git checkout --force "$2"
git submodule update --init --force
