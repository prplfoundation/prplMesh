ARG image=ubuntu:18.04
FROM $image

# Update Software repository
RUN apt-get update && apt-get install -y \
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
	vim \
	net-tools \
	bridge-utils \
	iputils-ping \
	iproute2 \
	psmisc \
    && rm -rf /var/lib/apt/lists/*

ARG bridgeip

ADD entrypoint.sh /root/
ADD start-controller /usr/bin
ADD start-agent /usr/bin
ADD start-controller-agent /usr/bin

ENTRYPOINT ["/root/entrypoint.sh"]
