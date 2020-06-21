###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2019-2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

####
## OpenWrt pre-requisites and "openwrt" user
####
FROM ubuntu:18.04 as openwrt-prerequisites

RUN apt-get update \
    && apt-get -y install --no-install-recommends \
    build-essential libncurses5 libncurses5-dev python python3 \
    unzip git ca-certificates gawk wget file bash \
    python-yaml python3-yaml rsync less vim gnupg software-properties-common \
    && rm -rf /var/lib/apt/lists/*
# Building tlvfs currently uses the docker
# container's python (not the one from OpenWrt)
# python-yaml is needed to build the tlvfs
# vim and less are there to debug builds at run time

RUN useradd -ms /bin/bash openwrt

####
## OpenWrt tree and scripts to build it.
####
FROM openwrt-prerequisites as openwrt-builder

USER openwrt

# The following args are mandatory, do not expect the build to
# succeed without specifying them on the command line.
#
# OpenWrt repository to use. Can also be a prplWrt repository:
ARG OPENWRT_REPOSITORY
ENV OPENWRT_REPOSITORY=$OPENWRT_REPOSITORY
# Which OpenWrt version (commit hash) to use:
ARG OPENWRT_VERSION
ENV OPENWRT_VERSION=$OPENWRT_VERSION
# The variant to build (nl80211 or dwpal)
ARG PRPLMESH_VARIANT
ENV PRPLMESH_VARIANT=$PRPLMESH_VARIANT

# The feed to use to build prplMesh.
ARG PRPL_FEED
ENV PRPL_FEED=$PRPL_FEED
# optional: intel feed to use.
ARG INTEL_FEED
ENV INTEL_FEED=$INTEL_FEED
# optional: iwlwav feed to use.
ARG IWLWAV_FEED
ENV IWLWAV_FEED=$IWLWAV_FEED

# Target to build for (CONFIG_TARGET_ will be prepended).
# Example: TARGET_SYSTEM=mvebu
ARG TARGET_SYSTEM
ENV TARGET_SYSTEM=$TARGET_SYSTEM

# Subtarget (CONFIG_TARGET_${TARGET_SYSTEM}_ will be prepended).
# Example: SUBTARGET=cortexa9
ARG SUBTARGET
ENV SUBTARGET=$SUBTARGET

# Target profile (CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}_ will be prepended).
# Example: TARGET_PROFILE=DEVICE_cznic_turris-omnia
ARG TARGET_PROFILE
ENV TARGET_PROFILE=$TARGET_PROFILE

WORKDIR /home/openwrt

RUN printf '\033[1;35m%s Cloning prplWrt\n\033[0m' "$(date --iso-8601=seconds --universal)" \
    && git clone "$OPENWRT_REPOSITORY" openwrt \
    && cd openwrt \
    && git checkout "$OPENWRT_VERSION"

WORKDIR /home/openwrt/openwrt

COPY --chown=openwrt:openwrt scripts/build-openwrt.sh /home/openwrt/openwrt/scripts/build-openwrt.sh

####
## Prebuilt OpenWrt and prplMesh dependencies, but not the prplMesh ipk itself
####
FROM openwrt-builder as prplmesh-builder

RUN scripts/build-openwrt.sh
