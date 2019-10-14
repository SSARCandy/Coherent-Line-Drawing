#!/bin/bash

PROGNAME=${0##*/}

CURDIR="$( cd "$(dirname "$0")" ; pwd -P )"
BUILD_TYPE="Release"
BUILD_DIR="build"

if which nproc; then
    # Linux
    CORES="$(nproc --all)"
elif which sysctl; then
    # MacOS
    CORES="$(sysctl -n hw.logicalcpu)"
else
    CORES=2
fi
JOBS=$(( CORES / 2 ))

usage() {

cat <<EOF

  Usage: $PROGNAME [options]

  Options:

    -h, --help        Display this help and exit
    -c, --clean       Clean build
    -d, --debug       Build with debug mode
    -j, --jobs        Use N cores to build

EOF
}

clean() {
    rm -rf "$BUILD_DIR"
}


while (( "$#" )); do
    case "$1" in
        -h|--help)
            usage
            exit 0
            ;;
        -j|--jobs)
            JOBS="$2"
            shift 2
            ;;
        -d|--debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        -c|--clean)
            clean
            shift
            ;;
        -*|--*=)
            echo "Invalid arguments"
            exit 1
            ;;
        *)
            break
            ;;
    esac
done

BUILD_OPTIONS="-DCMAKE_BUILD_TYPE=${BUILD_TYPE}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"
if cmake .. ${BUILD_OPTIONS}; then
    make -j "${JOBS}" && cd "$CURDIR"
else
    cd "$CURDIR"
    exit 1
fi