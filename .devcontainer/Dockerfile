#-------------------------------------------------------------------------------------------------------------
# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License. See https://go.microsoft.com/fwlink/?linkid=2090316 for license information.
#-------------------------------------------------------------------------------------------------------------
FROM mcr.microsoft.com/vscode/devcontainers/base:ubuntu-18.04

# Avoid warnings by switching to noninteractive
ENV DEBIAN_FRONTEND=noninteractive

# Install build essintials
RUN apt-get update \
    #
    # Install C++ tools
    && apt-get -y install build-essential cmake cppcheck valgrind \
    # Libraries
    libjson-c-dev python-yaml python3-yaml libzmq3-dev libssl-dev \
    libncurses-dev libreadline-dev libnl-3-dev libnl-route-3-dev libnl-genl-3-dev \
    # Install Helper tools
    bash-completion locales ninja-build pkg-config shellcheck \
    flake8 clang-format vim \
    # Network capturing
    tcpdump tshark wireshark-common openssh-server

# Install podman
RUN echo "deb https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_$(lsb_release -sr)/ /" \
         > /etc/apt/sources.list.d/devel:kubic:libcontainers:stable.list \
    && curl -L "https://download.opensuse.org/repositories/devel:/kubic:/libcontainers:/stable/xUbuntu_$(lsb_release -sr)/Release.key" | apt-key add - \
    && apt-get update && apt-get install -y podman dnsmasq \
    && ln -s /usr/bin/podman /usr/bin/docker

# Install docker-ce
# RUN apt-get -y install apt-transport-https ca-certificates \
#         curl gnupg-agent software-properties-common \
#     && curl -fsSL https://download.docker.com/linux/ubuntu/gpg | apt-key add - \
#     && apt-key fingerprint 0EBFCD88 \
#     && add-apt-repository \
#         "deb [arch=amd64] https://download.docker.com/linux/ubuntu \
#         $(lsb_release -cs) \
#         stable" \
#     && apt-get update \
#     && apt-get -y install docker-ce docker-ce-cli containerd.io

# Clean up
RUN apt-get autoremove -y \
    && apt-get clean -y \
    && rm -rf /var/lib/apt/lists/*

# Update locale
RUN locale-gen en_US.UTF-8 && update-locale

# Add GitHub SSH fingerprint to the known_hosts file
RUN mkdir -p /root/.ssh && ssh-keyscan github.com >> /root/.ssh/known_hosts

# Set default 'root' password
RUN echo 'root:prplmesh' | chpasswd

# Install configuration files
COPY scripts/etc/ssh/sshd_config /etc/ssh/
COPY scripts/etc/containers/libpod.conf /etc/containers/

# Install shell config files
COPY scripts/root/.bashrc /root/
COPY scripts/root/.inputrc /root/

# Switch back to dialog for any ad-hoc use of apt-get
ENV DEBIAN_FRONTEND=dialog

# Default user is root
ENV USER=root

# SSH Server
EXPOSE 2222
