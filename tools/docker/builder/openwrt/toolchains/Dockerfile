FROM ubuntu:16.04

RUN dpkg --add-architecture i386

RUN apt-get -y update && apt-get install --no-install-recommends -y sudo git-core \
gawk wget unzip make uml-utilities bridge-utils debootstrap gcc-multilib \
perl automake autoconf fakeroot build-essential crash makedumpfile \
kernel-wedge git-core libncurses5 libncurses5-dev libelf-dev asciidoc binutils-dev \
bison imagemagick openssl cmake git-core expect xutils-dev genext2fs m4 qemu \
libqt4-dev qt4-dev-tools python-setuptools \
python-pip python3.5 python3.5-dev apt-file gcc gcc-multilib \
libstdc++6:i386 python-zope.interface python-bjsonrpc \
python-crypto python-openssl python-twisted curl libcurl3 libcurl4-openssl-dev libjson0-dev \
fuse libfuse2 libfuse-dev libpcre3 libpcre3-dev zlib1g zlib1g-dev \
libidn2-0 libxml2-dev unzip subversion nmap python-qt4 cflow lua5.1 libssl1.0.0 libssl-dev

RUN pip install --upgrade pip && pip install pyyaml

COPY entrypoint.sh /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/bin/bash","/usr/local/bin/entrypoint.sh"]
