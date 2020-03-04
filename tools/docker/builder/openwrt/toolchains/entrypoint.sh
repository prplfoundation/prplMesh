#!/bin/sh

cmd="$@"

# Check if the command argument is a service command
if [ "${cmd}" = "show-launch-script" ] ; then
    cat /usr/local/bin/openwrt-env
    exit
fi

FILE_SYSTEM_OUTPUT=/fs-output
ROOT=/workspace
WORK_DIR=${ROOT}

# Create a user and his primary group that match a host's user inside a Docker
# container.
# Note: We don't create a user's home directory because it is already created by
# mounting a host's user .ssh directory in 'docker run' command.
groupadd -g ${GROUP_ID} ${GROUP_NAME}
useradd -g ${GROUP_ID} -s "/bin/bash" -u ${USER_ID} ${USER_NAME}

# Prepare user's home directory
cp -pr /etc/skel/. /home/${USER_NAME}

cat <<EOF >> /home/${USER_NAME}/.profile
PS1='OPENWRT-ENV:\w\$ '
export FILE_SYSTEM_OUTPUT=${FILE_SYSTEM_OUTPUT}
export ROOT=${ROOT}
cd ${WORK_DIR}

./scripts/feeds setup "src-git,packages,https://git.openwrt.org/feed/packages.git;openwrt-19.07" "src-git,intel_feed,https://git.prpl.dev/prplmesh/feed-intel.git;intel-19.07" "src-git,iwlwav,https://git.prpl.dev/prplmesh/iwlwav.git;iwlwav-19.07" "src-git,routing,https://github.com/openwrt-routing/packages.git" "src-git,luci,https://git.openwrt.org/project/luci.git^c0e73d3f9567f227cbe36ba12af53efbfdd4343d" "src-git,prpl_feed,https://git.prpl.dev/prplwrt/feed-prpl.git"
./scripts/feeds update
./scripts/feeds install -a -p intel_feed
./scripts/feeds install -a -p prpl_feed
./scripts/feeds install -a -p iwlwav
./scripts/feeds install -a -p luci
./scripts/feeds install intel_mips
./scripts/feeds install zmq
./scripts/gen_config.sh rax40 luci

EOF

chown -R ${USER_NAME}:${GROUP_NAME} /home/${USER_NAME}

# Run!
if [ $# -eq 0 ] ; then
    # No arguments were passed (interactive mode)
    sudo -i -u ${USER_NAME}
else
    # Some arguments were passed (non-interactive mode)
    sudo -i -u ${USER_NAME} -- "/bin/bash" -c "${cmd}"
fi

