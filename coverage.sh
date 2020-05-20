#!/bin/sh

# cd to the script directory:
cd "${0%/*}" || { echo "Couldn't cd to ${0%/*}!"; exit 1; }

export REPORTS_DIR=./build/reports
export COVERAGE_DIR=${REPORTS_DIR}/coverage

rm -rf ${COVERAGE_DIR} 
mkdir -p ${COVERAGE_DIR}

# See "Gcovr User Guide" for details on 'gcovr' usage
# https://gcovr.com/en/stable/guide.html
gcovr -r . --exclude='build/*' --exclude='framework/external/*' --exclude='.*/unit_tests/.*' --html --html-details -o ${COVERAGE_DIR}/coverage.html


