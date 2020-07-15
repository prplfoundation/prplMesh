#!/bin/bash
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.

scriptdir=$(dirname "$(readlink -f "${0}")")
bf_plugins_dir=${scriptdir}/boardfarm_plugins
bf_dir=$(realpath "${scriptdir}/../../boardfarm")

if [ -n "${PYTHONPATH}" ]; then
   PYTHONPATH="${bf_dir}:${bf_plugins_dir}:${scriptdir}:${PYTHONPATH}"
else
   PYTHONPATH="${bf_dir}:${bf_plugins_dir}:${scriptdir}"
fi
echo $PYTHONPATH
export PYTHONPATH

exec python3 "${bf_dir}"/bft -c "${bf_plugins_dir}"/boardfarm_prplmesh/prplmesh_config_compose.json -n prplmesh_docker -x test_flows
