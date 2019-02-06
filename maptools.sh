if [ ! -d "${CHDLAB_PATH}" ]; then
    echo "Error: CHDLAB_PATH not set, aborting."
    exit 1
fi

VIRTUAL_ENV_DISABLE_PROMPT=1 && source ${CHDLAB_PATH}/.virtualenv/bin/activate
./maptools.py $@
deactivate