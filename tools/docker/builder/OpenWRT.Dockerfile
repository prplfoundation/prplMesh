###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 Vitaly Bukhovsky (Devalore)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

FROM ubuntu:18.04

# Update Software repository
RUN apt-get update

# Essential build tools
RUN apt-get install -y build-essential git cmake gcc-multilib \
  subversion autoconf autogen libtool pkg-config

# Command line utilities
RUN apt-get install -y bzr cvs gawk flex gettext zip unzip

# Build support libraries
RUN apt-get install -y libncurses5-dev libssl-dev libxml-parser-perl \
  zlib1g-dev libjson-c-dev

# Python/Java
RUN apt-get install -y openjdk-8-jdk python3 python2.7-dev python-yaml

# Cleanup
RUN rm -rf /var/lib/apt/lists/*

# Platform specific arguments
ARG PRPLMESH_PLATFORM_TYPE
ARG PRPLMESH_PLATFORM_BASE_DIR
ARG PRPLMESH_PLATFORM_BUILD_NAME
ARG PRPLMESH_PLATFORM_TOOLCHAIN
ARG PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX

# Make sure that all build arguments are supplied
RUN  test -n "$PRPLMESH_PLATFORM_TYPE" \
  && test -n "$PRPLMESH_PLATFORM_BASE_DIR" \
  && test -n "$PRPLMESH_PLATFORM_BUILD_NAME" \
  && test -n "$PRPLMESH_PLATFORM_TOOLCHAIN" \
  && test -n "$PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX"

# Platform specific environment variables
ENV CMAKE_TOOLCHAIN_FILE=./prplMesh/tools/cmake/toolchain/openwrt.cmake
ENV PRPLMESH_PLATFORM_TYPE              $PRPLMESH_PLATFORM_TYPE
ENV PRPLMESH_PLATFORM_BASE_DIR          $PRPLMESH_PLATFORM_BASE_DIR
ENV PRPLMESH_PLATFORM_BUILD_NAME        $PRPLMESH_PLATFORM_BUILD_NAME
ENV PRPLMESH_PLATFORM_TOOLCHAIN         $PRPLMESH_PLATFORM_TOOLCHAIN
ENV PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX  $PRPLMESH_PLATFORM_TOOLCHAIN_PREFIX

# Default command
ENTRYPOINT ["./prplMesh/tools/maptools.py", "build"]
