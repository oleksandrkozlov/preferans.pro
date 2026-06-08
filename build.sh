#!/bin/bash -x

# SPDX-License-Identifier: AGPL-3.0-only
# Copyright (c) 2025 Oleksandr Kozlov

set -euo pipefail

WS_HOST=${1:-0.0.0.0}
WS_PROTOCOL=${2:-ws}
WS_PORT=${3:-8080}
BUILD_TYPE=${4:-Release}

if [[ "$BUILD_TYPE" == "Debug" ]]; then
    SERVER_DIR="build-dbg-server"
    CLIENT_DIR="build-dbg-client"
else
    SERVER_DIR="build-server"
    CLIENT_DIR="build-client"
fi

cmake -S server -B $SERVER_DIR -GNinja -DCMAKE_BUILD_TYPE=$BUILD_TYPE $( [[ "$WS_PROTOCOL" == "wss" ]] && echo "-DPREF_SSL=ON" )
cmake --build $SERVER_DIR --target server pref-cli
mkdir -p ./server/data
touch ./server/data/game.dat

source /etc/profile.d/emscripten.sh
emcmake cmake -S client -B $CLIENT_DIR -G "Unix Makefiles"  -DCMAKE_BUILD_TYPE=$BUILD_TYPE -Dprotobuf_BUILD_TESTS=OFF -DPLATFORM=Web -DCMAKE_PREF_HOST=$WS_HOST -DCMAKE_WEBSOCKET_URL=$WS_PROTOCOL://$WS_HOST:$WS_PORT -DCMAKE_POLICY_VERSION_MINIMUM=3.5
cmake --build $CLIENT_DIR -j `nproc` --target docopt fmt libprotobuf-lite raylib spdlog
cmake --build $CLIENT_DIR -j `nproc` --target client
