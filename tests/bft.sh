#!/bin/sh
scriptdir=$(dirname $(readlink -f $0))
bf_dir=$(realpath "$scriptdir/../../boardfarm")
bfp_dir=$scriptdir

PYTHONPATH="$bfp_dir" python3 "$bf_dir/bft" "$@"
