if [ ! -d "${CHDLAB_PATH}" ]; then
    echo "Error: CHDLAB_PATH not set, aborting."
    exit 1
fi

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
VIRTUAL_ENV_DISABLE_PROMPT=1 && source ${CHDLAB_PATH}/.virtualenv/bin/activate
${DIR}/maptools.py $@
deactivate
