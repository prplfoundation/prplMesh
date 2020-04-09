ARG image=ubuntu:18.04
###############################################################
# SPDX-License-Identifier: BSD-2-Clause-Patent
# Copyright (c) 2019 the prplMesh contributors (see AUTHORS.md)
# This code is subject to the terms of the BSD+Patent license.
# See LICENSE file for more details.
###############################################################

FROM $image

# Update Software repository
RUN apt-get update && apt-get install -y \
	curl \
	gcc \
	cmake \
	ninja-build \
	git \
	autoconf \
	autogen \
	libtool \
	pkg-config \
	gdb \
	binutils \
	libreadline-dev \
	libncurses-dev \
	libssl-dev \
	libjson-c-dev \
	libnl-genl-3-dev \
	libzmq3-dev \
	python \
	python-yaml \
	python3 \
	python3-yaml \
	vim \
	net-tools \
	bridge-utils \
	iputils-ping \
	iproute2 \
	psmisc \
	clang-format \
	tmux \
    && rm -rf /var/lib/apt/lists/*

ENTRYPOINT ["/bin/bash"]
