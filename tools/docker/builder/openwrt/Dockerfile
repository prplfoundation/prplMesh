####
## OpenWrt pre-requisites and "openwrt" user
####
FROM ubuntu:18.04 as openwrt-prerequisites

RUN apt-get update \
    && apt-get -y install --no-install-recommends \
    build-essential libncurses5 libncurses5-dev python python3 \
    unzip git ca-certificates gawk wget file bash \
    python-yaml python3-yaml rsync less vim gnupg software-properties-common \
    #   yq is available in a separate ppa (needs gnupg and software-properties-common):
    && if [ -n "$HTTP_PROXY" ]; then KEYSERVER_OPTIONS="--keyserver-options http-proxy=$HTTP_PROXY"; fi \
    && apt-key adv --keyserver keyserver.ubuntu.com $KEYSERVER_OPTIONS --recv-keys CC86BB64 \
    && add-apt-repository ppa:rmescandon/yq \
    && apt-get -y install --no-install-recommends yq \
    && rm -rf /var/lib/apt/lists/*
# Building tlvfs currently uses the docker
# container's python (not the one from OpenWrt)
# python-yaml is needed to build the tlvfs
# vim and less are there to debug builds at run time

RUN useradd -ms /bin/bash openwrt

####
## Prebuilt OpenWrt and prplMesh dependencies, but not the prplMesh ipk itself
####
FROM openwrt-prerequisites as prplmesh-builder

USER openwrt

# The following args are mandatory, do not expect the build to
# succeed without specifying them on the command line.
#
# OpenWrt repository to use. Can also be a prplWrt repository:
ARG OPENWRT_REPOSITORY
# Which OpenWrt version (commit hash) to use:
ARG OPENWRT_VERSION
# The variant to build (nl80211 or dwpal)
ARG PRPLMESH_VARIANT

# The feed to use to build prplMesh.
ARG PRPL_FEED
# optional: intel feed to use.
ARG INTEL_FEED
# optional: iwlwav feed to use.
ARG IWLWAV_FEED

# Target to build for (CONFIG_TARGET_ will be prepended).
# Example: TARGET_SYSTEM=mvebu
ARG TARGET_SYSTEM

# Subtarget (CONFIG_TARGET_${TARGET_SYSTEM}_ will be prepended).
# Example: SUBTARGET=cortexa9
ARG SUBTARGET

# Target profile (CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}_ will be prepended).
# Example: TARGET_PROFILE=DEVICE_cznic_turris-omnia
ARG TARGET_PROFILE

WORKDIR /home/openwrt

RUN git clone "$OPENWRT_REPOSITORY" openwrt \
    && cd openwrt \
    && git checkout "$OPENWRT_VERSION"

WORKDIR /home/openwrt/openwrt

COPY --chown=openwrt:openwrt profiles_feeds/ /home/openwrt/openwrt/profiles_feeds

RUN mkdir -p files/etc \
    #   We need to keep the hashes in the firmware, to later know if an upgrade is needed:
    && printf '%s=%s\n' "OPENWRT_REPOSITORY" "$OPENWRT_REPOSITORY" >> files/etc/prplwrt-version \
    && printf '%s=%s\n' "OPENWRT_VERSION" "$OPENWRT_VERSION" >> files/etc/prplwrt-version \
    && if [ "$TARGET_PROFILE" = DEVICE_NETGEAR_RAX40 ] ; then \
        #       Add prplmesh to the list of packages of the profile:
        sed -i 's/packages:/packages:\n  - prplmesh-dwpal/g' profiles/intel_mips.yml \
        && yq write --inplace profiles/intel_mips.yml feeds -f profiles_feeds/netgear-rax40.yml \
        && ./scripts/gen_config.py intel_mips \
        #       Installing intel feed doesn't correctly regenerate kernel .package-info
        #       force regeneration by removing it
        && rm -rf tmp \
        #       For some reason we have to run gen_config a second time to get a correct .config:
        && ./scripts/gen_config.py intel_mips \
        #       make sure intel's bridge-utils is the only one that gets installed:
        && rm -rf ./package/feeds/packages/bridge-utils \
        && scripts/feeds install -p feed_bridge_utils bridge-utils \
        && cat profiles_feeds/netgear-rax40.yml >> files/etc/prplwrt-version ;\
    else \
        cp feeds.conf.default feeds.conf \
        && echo "src-git prpl $PRPL_FEED" >> feeds.conf \
        && scripts/feeds update -a \
        && scripts/feeds install -a \
        && echo "CONFIG_TARGET_${TARGET_SYSTEM}=y" >> .config \
        && echo "CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}=y" >> .config \
        && echo "CONFIG_TARGET_${TARGET_SYSTEM}_${SUBTARGET}_${TARGET_PROFILE}=y" >> .config \
        && echo "CONFIG_PACKAGE_prplmesh${PRPLMESH_VARIANT}=y" >> .config \
        && make defconfig \
        && printf '%s=%s\n' "PRPL_FEED" "$PRPL_FEED" >> files/etc/prplwrt-version ;\
    fi ;\
    make -j$(nproc) \
    && make package/prplmesh/clean
# note that the result from diffconfig.sh with a minimal
# configuration has the 3 CONFIG_TARGET items we set here, but NOT
# the individual CONFIG_TARGET_${SUBTARGET} and
# CONFIG_TARGET_${TARGET_PROFILE}, which means we don't need to
# set them.
