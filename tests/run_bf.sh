#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

scriptdir=$(dirname "$(readlink -f "${0}")")
bf_plugins_dir=${scriptdir}/boardfarm_plugins
bf_dir=$(realpath "${scriptdir}/../../boardfarm")
resultdir=${scriptdir}/../logs/

if [ -n "${PYTHONPATH}" ]; then
   PYTHONPATH="${bf_dir}:${bf_plugins_dir}:${scriptdir}:${PYTHONPATH}"
else
   PYTHONPATH="${bf_dir}:${bf_plugins_dir}:${scriptdir}"
fi
export PYTHONPATH

bft -c "${bf_plugins_dir}"/boardfarm_prplmesh/prplmesh_config.json \
        -n prplmesh_docker -x test_flows -o "${resultdir}" \
	|| exit 255

failed_test_count=$(jq '.tests_fail' "${resultdir}"/test_results.json)
re='^[0-9]+$'
if ! [[ "$failed_test_count" =~ $re ]]; then
   echo "Unable to parse failed test count:" "$failed_test_count" \
   && exit 255
fi
if [[ "$failed_test_count" -gt 0 ]]; then
   printf '\033[1;31m%s\033[0m\n' "$failed_test_count tests failed!" \
   && exit 1
fi
