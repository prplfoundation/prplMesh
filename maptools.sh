DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
if [ ! -d "${CHDLAB_PATH}" ]; then
    echo "CHDLAB_PATH not set, not using chdlab and virtual env..."
    python3 ${DIR}/maptools.py $@
else
    VIRTUAL_ENV_DISABLE_PROMPT=1 && source ${CHDLAB_PATH}/.virtualenv/bin/activate
    ${DIR}/maptools.py $@
    deactivate
fi