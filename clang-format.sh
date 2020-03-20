#! /bin/sh
# cd to the script directory:
cd "${0%/*}" || { echo "Couldn't cd to ${0%/*}!"; exit 1; }
CLANG_FORMAT="clang-format"
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

git ls-files -- '*.c' '*.cpp' '*.h' '*.hpp' '*.cc' | $filter | xargs clang-format -i -style=file
