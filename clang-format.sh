#! /bin/sh
cd ${0%/*}
CLANG_FORMAT="clang-format-3.9"
if ! [ -x "$(command -v ${CLANG_FORMAT})" ]; then
  echo "{$CLANG_FORMAT} not found. skipping..."
  exit 1
fi

IGNORE_FILE=".clang-ignore"
filter="grep ."
if test -f "$IGNORE_FILE"; then
    filter="grep -v -E -f .clang-ignore"
else
    filter="grep ."
fi

find */ -type f -name \*.[ch] -o -name \*.[ch]pp -o -name *.cc | \
    $filter | \
    xargs clang-format-3.9 -i -style=file