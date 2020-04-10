###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# SPDX-FileCopyrightText: 2020 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

FROM ubuntu:xenial

# Update Software repository
RUN apt-get update \
    && apt-get install -y \
    build-essential \
    git \
    cmake \
    gcc-multilib \
    subversion \
    autoconf \
    autogen \
    libtool \
    pkg-config \
    bzr \
    cvs \
    gawk \
    flex \
    gettext \
    zip \
    unzip \
    connect-proxy \
    libncurses5-dev \
    libssl-dev \
    libxml-parser-perl \
    zlib1g-dev \
    libjson-c-dev \
    repo \
    diffstat \
    texinfo \
    chrpath \
    wget \
    cpio \
    openssl \
    python3 \
    python2.7-dev \
    python-yaml \
    locales \
    && rm -rf /var/lib/apt/lists/*

# https://wiki.yoctoproject.org/wiki/TipsAndTricks/ResolvingLocaleIssues
RUN dpkg-reconfigure locales \
    && locale-gen en_US.UTF-8 \
    && update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
ENV LANG=en_US.UTF-8

ADD entrypoint.sh /
ENTRYPOINT ["/bin/bash", "/entrypoint.sh"]
