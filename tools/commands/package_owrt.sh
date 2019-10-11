#!/bin/bash -e

TGT_DST_DIR=/opt/beerocks

if [ "$1" = "" ] || [ "$1" = "-h" ]; then
  echo "Usage: $0 <USER@IP | -p | -h> [PASSWORD]"
  echo "Command line options and parameters:"
  echo "USER@IP - ssh/scp commands target (e.g. root@192.168.0.1)"
  echo "          the binaries tarball is uploaded to /tmp and extracted"
  echo "          into $TGT_DST_DIR on the target;"
  echo "-p - only create stripped binaries tarball for deployment;"
  echo "-h - show this usage message;"
  echo "PASSWORD - optional password for ssh/scp access to the target,"
  echo "           the default password is 'admin'."
  echo "You'll need sshpass for ssh/scp access."
  echo "For ubuntu: sudo apt-get install sshpass"
  exit 1
fi

# where to connnect (i.e. root@1.2.3.4)
UIP=${1}
# what password to try
PASSWD=${2-admin}

# go to the build folder
MY_DIR=$(dirname "$0")
MY_NAME=$(basename "$0")
cd "$MY_DIR/../../../build"
WORK_DIR=$PWD

# options for using with ssh and scp
SSHOPT="-q -o UserKnownHostsFile=/dev/null -o StrictHostKeyChecking=no"

# Check the install folder
if [ ! -e "$WORK_DIR/install" ]; then
  echo "Can't find: $WORK_DIR/install"
  cd -
  exit 2
fi

# figure out what strip to use
if [ "$OPENWRT_DIR" == "" ]; then
  echo "OPENWRT_DIR variable is not set"
  cd -
  exit 3
fi
if [ ! -e "$OPENWRT_DIR/.config" ]; then
  echo "Can't find: $OPENWRT_DIR/.config"
  cd -
  exit 4
fi
eval `grep CONFIG_ARCH= "$OPENWRT_DIR"/.config`
eval `grep CONFIG_CPU_TYPE= "$OPENWRT_DIR"/.config`
eval `grep CONFIG_GCC_VERSION= "$OPENWRT_DIR"/.config`
eval `grep CONFIG_LIBC= "$OPENWRT_DIR"/.config`
TOOLCHAIN_DIR=`ls -ldf "$OPENWRT_DIR"/staging_dir/toolchain-${CONFIG_ARCH}_${CONFIG_CPU_TYPE}_gcc-${CONFIG_GCC_VERSION}_${CONFIG_LIBC}*`
if [ ! -e "$TOOLCHAIN_DIR" ]; then
  echo "Can't find: $TOOLCHAIN_DIR"
  cd -
  exit 5
fi
STRIP=`ls -lf "$TOOLCHAIN_DIR"/bin/*strip | head -1`
if [ "$STRIP" == "" ] || [ ! -e "$STRIP" ]; then
  echo "Can't find strip under $TOOLCHAIN_DIR/bin/"
  cd -
  exit 6
fi

# copy the files over to a work folder
rm -Rf "$WORK_DIR/install-stripped"
cp -a "$WORK_DIR/install" "$WORK_DIR/install-stripped"
# get rid of unnecessary stuff
rm -Rf "$WORK_DIR/install-stripped/host" "$WORK_DIR/install-stripped/include"
rm -Rf "$WORK_DIR/install-stripped/lib/cmake"
# strip all binaries and libs
for _f in `find "$WORK_DIR/install-stripped" -type f`; do
  if file "$_f" | grep -E 'ELF .*not stripped' > /dev/null; then
    $STRIP --strip-unneeded "$_f"
  fi
done

# tar everything from the work folder
echo "tar -C \"$WORK_DIR/install-stripped\" -czvf \"$WORK_DIR/install-stripped.tgz\" ."
tar -C "$WORK_DIR/install-stripped" -czvf "$WORK_DIR/install-stripped.tgz" .
echo "Beerocks tar archive for upload:"
ls -l "$WORK_DIR/install-stripped.tgz"

# if using -p option, we are done
if [ "$UIP" == "-p" ]; then
  cd -
  exit 0
fi

# deploy to target
echo "Stopping map on the target (if exists)..."
sshpass -p "$PASSWD" ssh $SSHOPT $UIP \
        -C "[ ! -e $TGT_DST_DIR/scripts/prplmesh_utils.sh ] || $TGT_DST_DIR/scripts/prplmesh_utils.sh stop || true"
echo "Copying the tarball over to /tmp..."
sshpass -p "$PASSWD" scp $SSHOPT "$WORK_DIR/install-stripped.tgz" $UIP:/tmp/
echo "Cleaning up old binaries..."
sshpass -p "$PASSWD" ssh $SSHOPT $UIP -C "rm -Rf $TGT_DST_DIR"
echo "Extracting the new binaries into $TGT_DST_DIR..."
sshpass -p "$PASSWD" ssh $SSHOPT $UIP \
        -C "mkdir -p $TGT_DST_DIR && cd $TGT_DST_DIR && tar -xzvf /tmp/install-stripped.tgz"
echo "If everything worked so far, run the following command on the target to start:"
echo "$TGT_DST_DIR/scripts/prplmesh_utils.sh start"
