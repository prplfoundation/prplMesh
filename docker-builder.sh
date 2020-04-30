#!/bin/bash

# shellcheck source=tools/functions.sh
. "$(dirname "${BASH_SOURCE[0]}")/tools/functions.sh"

"${ROOT_DIR}/tools/docker/build.sh" "$@"
