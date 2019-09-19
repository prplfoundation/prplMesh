###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

FROM ubuntu:16.04

# Update Software repository
RUN apt-get update \
  && apt-get install -y software-properties-common \
  && add-apt-repository ppa:git-core/ppa \
  && apt-get update

# Essential build tools
RUN apt-get install -y build-essential git cmake gcc-multilib \
  subversion autoconf autogen libtool pkg-config

# Command line utilities
RUN apt-get install -y bzr cvs gawk flex gettext zip unzip wget \
  nano vim rsync

# Build support libraries
RUN apt-get install -y libncurses5-dev libssl-dev libxml-parser-perl \
  zlib1g-dev libjson-c-dev

# Python/Java
RUN apt-get install -y openjdk-8-jdk python3 python2.7-dev python-yaml

# Cleanup
RUN rm -rf /var/lib/apt/lists/*