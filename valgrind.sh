#!/bin/sh

VALGRIND="valgrind"
if ! [ -x "$(command -v ${VALGRIND})" ]; then
  echo "{$VALGRIND} not found. skipping..."
  exit 1
fi

valgrind --error-exitcode=1 --tool=memcheck --leak-check=full --track-origins=yes -q --error-limit=no --gen-suppressions=all "$@"
