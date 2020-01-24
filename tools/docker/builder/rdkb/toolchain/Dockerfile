###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2020 Tomer Eliyahu (Intel)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

FROM ubuntu:xenial

# Update Software repository
RUN apt-get update

# Essential build tools
RUN apt-get install -y build-essential git cmake gcc-multilib \
  subversion autoconf autogen libtool pkg-config

# Command line utilities
RUN apt-get install -y bzr cvs gawk flex gettext zip unzip connect-proxy

# Build support libraries
RUN apt-get install -y libncurses5-dev libssl-dev libxml-parser-perl \
  zlib1g-dev libjson-c-dev repo diffstat texinfo chrpath wget cpio

RUN apt-get install openssl
# Python/Java
RUN apt-get install -y openjdk-8-jdk python3 python2.7-dev python-yaml

# https://wiki.yoctoproject.org/wiki/TipsAndTricks/ResolvingLocaleIssues
RUN apt-get install -y locales
RUN dpkg-reconfigure locales
RUN locale-gen en_US.UTF-8
RUN update-locale LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8
ENV LANG=en_US.UTF-8

# Cleanup
RUN rm -rf /var/lib/apt/lists/*

RUN useradd -ms /bin/bash rdkb

USER rdkb
WORKDIR /home/rdkb

ADD entrypoint.sh /
ENTRYPOINT ["/bin/bash", "/entrypoint.sh"]
