<div align="center">

<a href="https://preferans.pro"><img src="client/resources/images/title.png" width="50%"></a>

![Preferans](https://repository-images.githubusercontent.com/1111279870/c43d87f8-c852-40dd-8c5e-9e579ed8603b)

Web Multiplayer Card Game in C++
--------------------------------

</div>

The server is implemented using `Boost.Beast` (for `WebSocket`) and
`Boost.Asio` for networking, using `stdexec` for structured concurrency. The
client is compiled to `WebAssembly` via `Emscripten`, using `WebSockets` for
communication. Rendering is handled with `raylib`/`raylib-cpp`, and the GUI is
built with `raygui`. Both server and client use `Protobuf` for message
serialization. Voice communication between players is implemented using
`WebRTC`.

### Docker

```
git submodule update --init --recursive
xhost +local:docker
DOCKER_BUILDKIT=0 docker build --pull --tag preferans .
```

#### Linux

```
docker run --privileged -ti -e DISPLAY=$DISPLAY \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $HOME:$HOME -w $PWD --network host --name preferans preferans
```

#### macOS

```
docker run --privileged -ti -e DISPLAY=host.docker.internal:0 \
    -v /tmp/.X11-unix:/tmp/.X11-unix \
    -v $HOME:$HOME -w $PWD -p 8000:8000 -p 8080:8080 -p 8081:8081 --name preferans preferans
```

### Build

```
./build.sh [<host>] [(ws | wss)] [<ws-port>] [(Debug | Release)]
```
Example:
```
./build.sh
```
Equivalent to:
```
./build.sh 0.0.0.0 ws 8080 Debug
```
For Release:
```
./build.sh preferans.pro wss 8080 Release
```

### Add User

```
cmake --build <build> --target pref-cli
./<build>/bin/pref-cli ./server/data/game.dat add --user <name> <password>
```

### Run

```
./run.sh [<ip-address>] [<http-port>] [<ws-port>] \
    [--cert=<fullchain.pem> --key=<privkey.pem> --dh=<ssl-dhparams.pem>]
```
Example:
```
./run.sh
```
Equivalent to:
```
./run.sh 0.0.0.0 8000 8080
```
For Release:
* Obtain and install your TLS certificates using [Certbot Instructions | Certbot](https://certbot.eff.org/instructions?ws=nginx&os=pip) and [configure](server/config/preferans.pro) Nginx accordingly.
* Start the HTTPS server:
```
nginx
```
* Start the WebSocket and HTTP server:
```
nohup ./<build>/bin/server <ip-address> 8080 ./server/data/game.dat \
    --cert=/path/to/fullchain.pem \
    --key=/path/to/privkey.pem \
    --dh=/path/to/ssl-dhparams.pem &
```
```
python3 ./tools/prefbuff/server.py ./server/data/game.dat \
    --host 127.0.0.1 --port 8081  > prefbuff.log 2>&1 &
```

### Test

Requires a Debug build.
```
cmake --build <build> --target test_server
./<build>/bin/test_server
./tests/run-tests.sh
```

### Dependencies

* [boost.asio](https://www.boost.org/doc/libs/latest/doc/html/boost_asio.html)
* [boost.beast](https://www.boost.org/doc/libs/latest/libs/beast/doc/html/index.html)
* [botan](https://botan.randombit.net/)
* [cmake](https://cmake.org)
* [date](https://howardhinnant.github.io/date/date.html)
* [docopt](http://docopt.org/)
* [emscripten](https://emscripten.org)
* [fmt](https://fmt.dev)
* [gsl](https://github.com/microsoft/GSL)
* [openssl](https://www.openssl.org/)
* [protobuf](https://protobuf.dev)
* [range-v3](https://ericniebler.github.io/range-v3/)
* [raygui](https://github.com/raysan5/raygui)
* [raylib-cpp](https://robloach.github.io/raylib-cpp/)
* [raylib](https://www.raylib.com)
* [spdlog](https://gabime.github.io/spdlog/)
* [stdexec](https://github.com/NVIDIA/stdexec)
* [wasm](https://webassembly.org)
* [webrtc](https://webrtc.org/)

### License

[![License: AGPL v3](https://img.shields.io/badge/License-AGPL_v3-blue.svg)](LICENSE)

The main source code of this project is licensed under AGPL-3.0-only.
Bundled assets keep their original licenses and are **not** covered by AGPL.

- **Cards**: atlas  - [CC0](client/resources/cards/atlas/LICENSE), poker - [Public Domain](client/resources/cards/poker/LICENSE), simple - [CC0](client/resources/cards/simple/LICENSE)
- **Fonts**: [DejaVu / Font Awesome Free (OFL)](client/resources/fonts/LICENSE)
- **Shell**: [MIT](client/resources/html/LICENSE)
- **Styles**: [zlib / AGPL-3.0-only](client/resources/styles/LICENSE)

© 2025 Oleksandr Kozlov — AGPL-3.0-only
