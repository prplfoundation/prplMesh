FROM alpine:3.11.3

RUN apk add --update --no-cache \
    autoconf \
    binutils \
    bridge-utils \
    cmake \
    cppcheck \
    curl \
    gcc \
    g++ \
    git \
    json-c-dev \
    libnl3-dev \
    libtool \
    musl-dev \
    zeromq-dev \
    libzmq \
    linux-headers \
    make \
    ncurses-dev \
    ninja \
    openssl-dev \
    pkgconf \
    py2-pip \
    python2 \
    readline-dev \
    && apk add --update --no-cache --repository=http://dl-cdn.alpinelinux.org/alpine/edge/main \
    clang-extra-tools
# clang-extra-tools needs to be fetched from the edge repository for now

RUN pip install pyyaml
