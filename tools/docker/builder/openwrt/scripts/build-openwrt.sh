#!/bin/sh -e
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

printf '\033[1;35m%s Configuring prplWrt\n\033[0m' "$(date --iso-8601=seconds --universal)"
mkdir -p files/etc
#   We need to keep the hashes in the firmware, to later know if an upgrade is needed:
printf '%s=%s\n' "OPENWRT_REPOSITORY" "$OPENWRT_REPOSITORY" >> files/etc/prplwrt-version
printf '%s=%s\n' "OPENWRT_VERSION" "$OPENWRT_VERSION" >> files/etc/prplwrt-version
if [ "$TARGET_PROFILE" = DEVICE_NETGEAR_RAX40 ] ; then
    # Add prplmesh to the list of packages of the profile:
    sed -i 's/packages:/packages:\n  - prplmesh-dwpal/g' profiles/intel_mips.yml
    yq write --inplace profiles/intel_mips.yml feeds -f profiles_feeds/netgear-rax40.yml
    ./scripts/gen_config.py intel_mips
    # Installing intel feed doesn't correctly regenerate kernel .package-info
    # force regeneration by removing it
    rm -rf tmp
    # For some reason we have to run gen_config a second time to get a correct .config:
    ./scripts/gen_config.py intel_mips
    #       make sure intel's bridge-utils is the only one that gets installed:
    rm -rf ./package/feeds/packages/bridge-utils
    scripts/feeds install -p feed_bridge_utils bridge-utils
    cat profiles_feeds/netgear-rax40.yml >> files/etc/prplwrt-version
else
    cp feeds.conf.default feeds.conf
    echo "src-git prpl $PRPL_FEED" >> feeds.conf
    scripts/feeds update -a
    scripts/feeds install -a
    {
        # note that the result from diffconfig.sh with a minimal
        # configuration has the 3 CONFIG_TARGET items we set here, but NOT
        # the individual CONFIG_TARGET_${SUBTARGET} and
        # CONFIG_TARGET_${TARGET_PROFILE}, which means we don't need to
        # set them.
        echo "CONFIG_TARGET_${TARGET_SYSTEM}=y"
        echo "CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}=y"
        echo "CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}_${TARGET_PROFILE}=y"
        echo "CONFIG_PACKAGE_prplmesh${PRPLMESH_VARIANT}=y"
    } >> .config
    make defconfig
    printf '%s=%s\n' "PRPL_FEED" "$PRPL_FEED" >> files/etc/prplwrt-version
fi
printf '\033[1;35m%s Building prplWrt\n\033[0m' "$(date --iso-8601=seconds --universal)"
make -j"$(nproc)" V=sc -Orecurse

printf '\033[1;35m%s **ls -l host/bin **\n\033[0m' "$(date --iso-8601=seconds --universal)"
ls -l staging_dir/host/bin

printf '\033[1;35m%s Cleaning prplMesh\n\033[0m' "$(date --iso-8601=seconds --universal)"
make package/prplmesh/clean
