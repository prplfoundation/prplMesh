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
## Pre-built OpenWrt (and extracted sdk)
####
FROM openwrt-prerequisites as openwrt-prebuilt

USER openwrt

# The following args are mandatory, do not expect the build to
# succeed without specifying them on the command line.
#
# OpenWrt repository to use. Can also be a prplWrt repository:
ARG OPENWRT_REPOSITORY
# Which OpenWrt version (commit hash) to use:
ARG OPENWRT_VERSION
# Target to build for (can be any existing value after CONFIG_TARGET_):
ARG TARGET

WORKDIR /home/openwrt

RUN git clone "$OPENWRT_REPOSITORY" openwrt \
    && cd openwrt \
    && git checkout "$OPENWRT_VERSION"

WORKDIR /home/openwrt/openwrt

RUN echo "CONFIG_SDK=y" > .config \
    && echo "CONFIG_TARGET_${TARGET}=y" >> .config \
    && make defconfig \
    && make -j$(nproc)

RUN TAR="$(find bin/targets -name 'openwrt-sdk*' -print)" \
    && mkdir -p "/home/openwrt/openwrt_sdk" \
    && tar -C "/home/openwrt/openwrt_sdk" -xf "${TAR}" --strip-components=1

####
## Prebuilt sdk only
####
FROM openwrt-prerequisites as openwrt-sdk
COPY --from=openwrt-prebuilt --chown=openwrt:openwrt /home/openwrt/openwrt_sdk /home/openwrt/openwrt_sdk
WORKDIR /home/openwrt/openwrt_sdk

####
## Prebuilt sdk and prplmesh dependencies
####

FROM openwrt-sdk as prplmesh-builder

COPY --chown=openwrt:openwrt configs/prplmesh_config /home/openwrt/prplmesh_config

# The following args are mandatory, do not expect the build to
# succeed without specifying them on the command line.
#
# TARGET_PLATFORM_TYPE currently needs to be specified, but will be
# removed in the future.
ARG TARGET_PLATFORM_TYPE
# The feed to use to build prplMesh.
ARG PRPL_FEED

USER openwrt

RUN echo "src-git prpl ${PRPL_FEED}" >> feeds.conf.default \
    && scripts/feeds update -a \
    && scripts/feeds install -a \
    && cp "/home/openwrt/prplmesh_config" .config \
    && make defconfig \
    && make -j$(nproc) V=sc \
    && make -j$(nproc) \
    && make package/feeds/prpl/prplmesh/clean
    # For some reason, make has to be run twice because
    # it still rebuilds some packages on the second run.
    # On the third run, it's able to compile prplmesh only.
