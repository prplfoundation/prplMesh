####
## OpenWrt pre-requisites and "openwrt" user
####
FROM ubuntu:18.04 as openwrt-prerequisites

RUN apt-get update \
    && apt-get -y install --no-install-recommends \
    build-essential libncurses5 libncurses5-dev python python3 \
    unzip git ca-certificates gawk wget file bash \
    python-yaml rsync less vim \
    && rm -rf /var/lib/apt/lists/*
    # Building tlvfs currently uses the docker
    # container's python (not the one from OpenWrt)
    # python-yaml is needed to build the tlvfs
    # vim and less are there to debug builds at run time

RUN useradd -ms /bin/bash openwrt

####
## Pre-built OpenWrt
####
FROM openwrt-prerequisites as openwrt-prebuilt

USER openwrt

# optional: intel feed to use.
ARG INTEL_FEED

# The following args are mandatory, do not expect the build to
# succeed without specifying them on the command line.
#
# OpenWrt repository to use. Can also be a prplWrt repository:
ARG OPENWRT_REPOSITORY
# Which OpenWrt version (commit hash) to use:
ARG OPENWRT_VERSION

# Target to build for (CONFIG_TARGET_ will be prepended).
# Example: TARGET_SYSTEM=mvebu
ARG TARGET_SYSTEM

# Subtarget (CONFIG_TARGET_${TARGET_SYSTEM}_ will be prepended).
# Example: SUBTARGET=cortexa9
ARG SUBTARGET

# Target profile (CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}_ will be prepended).
# Example: TARGET_PROFILE=DEVICE_cznic_turris-omnia
ARG TARGET_PROFILE

# .config file to use as base. Can include be used to specify a custom
# toolchain for example. It has to be in the "configs" directory.
ARG BASE_CONFIG=default

WORKDIR /home/openwrt

RUN git clone "$OPENWRT_REPOSITORY" openwrt \
    && cd openwrt \
    && git checkout "$OPENWRT_VERSION"

WORKDIR /home/openwrt/openwrt

COPY --chown=openwrt:openwrt configs/base_${BASE_CONFIG}_config /home/openwrt/base_config

RUN if [ -n "$INTEL_FEED" ] ; then \
        echo "src-git intel $INTEL_FEED" >> feeds.conf.default \
        && scripts/feeds update intel \
        && scripts/feeds install intel_mips ; \
    fi ;\
    cp /home/openwrt/base_config .config \
    && echo "CONFIG_TARGET_${TARGET_SYSTEM}=y" >> .config \
    && echo "CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}=y" >> .config \
    && echo "CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}_${TARGET_PROFILE}=y" >> .config \
    && make defconfig \
    && cat .config \
    && make -j$(nproc) V=sc
    # note that the result from diffconfig.sh with a minimal
    # configuration has the 3 CONFIG_TARGET items we set here, but NOT
    # the individual CONFIG_TARGET_${SUBTARGET} and
    # CONFIG_TARGET_${TARGET_PROFILE}, which means we don't need to
    # set them.

####
## Prebuilt openwrt and prplmesh dependencies
####

FROM openwrt-prebuilt as prplmesh-builder

# The following args are mandatory, do not expect the build to
# succeed without specifying them on the command line.
#
# TARGET_PLATFORM_TYPE currently needs to be specified, but will be
# removed in the future.
ARG TARGET_PLATFORM_TYPE
# The feed to use to build prplMesh.
ARG PRPL_FEED
# The variant to build (nl80211 or dwpal)
ARG PRPLMESH_VARIANT

USER openwrt

# We first install from the intel feed preferably, because
# it replaces some packages from the "packages" feed.
# If no intel feed is present, nothing will be done so it's still ok.
RUN echo "src-git prpl ${PRPL_FEED}" >> feeds.conf.default \
    && scripts/feeds update -a \
    && scripts/feeds install -a -p intel \
    && scripts/feeds install -a \
    && echo "CONFIG_PACKAGE_prplmesh${PRPLMESH_VARIANT}=y" >> .config \
    && make defconfig \
    && cat .config \
    && make package/feeds/prpl/prplmesh/compile -j$(nproc) V=sc \
    && make package/feeds/prpl/prplmesh/compile -j$(nproc) \
    && make package/feeds/prpl/prplmesh/clean
    # For some reason, make has to be run twice because
    # it still rebuilds some packages on the second run.
    # On the third run, it's able to compile prplmesh only.
    # V=sc turns on verbosity for the first command to
    # make debugging easier.
