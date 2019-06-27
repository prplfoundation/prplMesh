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
	net-tools \
	bridge-utils \
	iputils-ping \
	iproute2 \
	sudo \
	psmisc \
    && rm -rf /var/lib/apt/lists/*

ARG uid
ARG gid
ARG installdir
ARG bridgeip

RUN useradd -d /home/prpl -m -s /bin/bash prpl
RUN echo "prpl ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/prpl
RUN chmod 0440 /etc/sudoers.d/prpl
ENV USER prpl

USER prpl
WORKDIR $installdir
ADD setup-network.sh /home/prpl/
ENTRYPOINT ["/home/prpl/setup-network.sh"]
