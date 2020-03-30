#!/bin/bash

scriptdir="$(cd "${0%/*}" && pwd)"
rootdir="${scriptdir%/*/*}"

# shellcheck source=../../tools/functions.sh
. "${rootdir}/tools/functions.sh"


# generate the list of current issues in the same format as the known issues:
"$rootdir"/tools/docker/static-analysis/cppcheck.sh "$rootdir" "--template='{file}: {severity}: {message} [{id}]{code}'" > /dev/null 2>&1

mapfile -t issues < <(grep -E ".+: .+: .+ \[.+\] .*$"  "$rootdir/cppcheck_results.txt")

status=0
for i in "${issues[@]}"
do
    if ! grep -q -F "$i" "$rootdir"/ci/cppcheck/cppcheck_existing_issues.txt ; then
        status=1
        err "New issue:"
        grep -F -A2 "$i" "$rootdir/cppcheck_results.txt"
    fi
done

exit $status
