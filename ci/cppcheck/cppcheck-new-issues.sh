#!/bin/bash

# shellcheck source=../../tools/functions.sh
. "$(dirname "${BASH_SOURCE[0]}")/../../tools/functions.sh"

# generate the list of current issues in the same format as the known issues:
"$ROOT_DIR"/tools/docker/static-analysis/cppcheck.sh "$ROOT_DIR" "--template='{file}: {severity}: {message} [{id}]{code}'" > /dev/null 2>&1

mapfile -t issues < <(grep -E ".+: .+: .+ \[.+\] .*$"  "$ROOT_DIR/cppcheck_results.txt")

status=0
for i in "${issues[@]}"
do
    if ! grep -q -F "$i" "$ROOT_DIR"/ci/cppcheck/cppcheck_existing_issues.txt ; then
        status=1
        err "New issue:"
        grep -F -A2 "$i" "$ROOT_DIR/cppcheck_results.txt"
    fi
done

exit $status
