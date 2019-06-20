FROM ubuntu:18.04

# Update Software repository
RUN apt-get update && apt-get install -y \
	curl \
	gcc \
	cmake \
	ninja-build \
	binutils \
	git \
	autoconf \
	autogen \
	libtool \
	pkg-config \
	libreadline-dev \
	libncurses-dev \
	libssl-dev \
	libjson-c-dev \
	libnl-genl-3-dev \
	libzmq3-dev \
	python \
	python-yaml \
	vim \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /root/prplMesh
ENTRYPOINT ["/root/prplMesh/tools/maptools.py", "build"]
