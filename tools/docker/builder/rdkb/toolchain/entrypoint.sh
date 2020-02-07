#!/bin/bash

echo RDKB_ROOT="$RDKB_ROOT"
echo USER="$USER"
echo USER_ID="$USER_ID"
echo GROUP_ID="$GROUP_ID"

groupadd -g "${GROUP_ID}" "${USER}"
useradd -l -u "${USER_ID}" -g "${USER}" "${USER}"
su "${USER}" <<EOSU
if ! cd "${RDKB_ROOT}"/setup ; then echo "Failed to cd to ${RDKB_ROOT}/setup"; exit 1; fi

if [ -d "${RDKB_ROOT}/atom_rdkbos/build" ]; then 
    . puma_setup -o 
else
    . puma_setup -c configs/p7_atom_rdkbos_wave600_dev-debug.config
fi

echo bitbake $@
bitbake $@
EOSU
