#!/bin/bash
echo RDKB_ROOT=$RDKB_ROOT
cd $RDKB_ROOT/setup

if [ -d "${RDKB_ROOT}/atom_rdkbos/build" ]; then 
    . puma_setup -o 
else
    . puma_setup -c configs/p7_atom_rdkbos_wave600_dev-debug.config
fi

exec "$@"
