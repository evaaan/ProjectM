# ProjectM

ProjectM is a 2D multiplayer game engine with an Entity-Component-System framework. Clients send their keyboard inputs to the server, which processes the game world and responds to each client with a set of Entity-Component updates. Clients read these messages, update their world state, and render the world for the user. The core framework is inspired by the [Nomad Game Engine](https://savas.ca/nomad).

This project is an exercise to learn game development and C++. For a current picture of things, check out [assets/screenshots/](assets/screenshots/).

Directory structure is as follows:
* The [client/](client/) folder has the Client code
* The [server/](server/) folder has the Server code
* The [test/](test/) folder has unit tests for both client and server
* The [common/](common/) folder has shared code used by both Client and Server
* The [lib/](lib/) folder has third party dependencies

Note: The client, server, and test folders have their own Visual Studio ``.vcxproj`` 'Project' file(s), under the [``ProjectM.sln``](ProjectM.sln) 'Solution' file in the root directory.

Tasks are tracked on a [Trello Board](https://trello.com/invite/b/cWteNm74/bf64646388becf6430bf7d5b8bd4df55/projectm).

# Install Prerequisites (Windows / Visual Studio)

1. Install [Visual Studio](https://visualstudio.microsoft.com/downloads/). In the Visual Studio Installer, select workloads for Desktop development with C++ and Game development with C++

The project uses Visual Studio and the MSVC compiler due to Windows APIs used (Windows, DirectX, COM, user input, ..)

2. Clone this project and update submodules:

```
git clone https://github.com/evaaan/ProjectM.git
cd ProjectM
git submodule update --init lib/GameNetworkingSockets
git submodule update --init lib/vcpkg
```

3. Change to the [lib/](lib/) directory:

```
cd lib
```

4. Install [Microsoft/vcpkg](https://github.com/Microsoft/vcpkg) with VS integration:

```
./vcpkg/bootstrap-vcpkg.bat
./vcpkg/vcpkg integrate install
```

5. Install [ValveSoftware/GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets):

```
./vcpkg/vcpkg --overlay-ports=GameNetworkingSockets/vcpkg_ports --triplet x64-windows install gamenetworkingsockets
```

6. Install [Google FlatBuffers](https://google.github.io/flatbuffers/index.html):

```
./vcpkg/vcpkg install --triplet x64-windows flatbuffers
```

Visual Studio should recognize header includes for all vcpkg-installed projects (under ``/lib/vcpkg/installed/x64-windows/include/``).

```
#include <GameNetworkingSockets/steam/steamnetworkingsockets.h>
#include <flatbuffers/flatbuffers.h>
```

May need to add flatc.exe from ``vcpkg\packages\flatbuffers_x64-windows\tools\flatbuffers\`` to your PATH.

7. Install [Boost](https://www.boost.org/users/download/) and add to C/C++ > General > Additional Include Directories.

8. Install [yaml-cpp](https://github.com/jbeder/yaml-cpp)  with vcpkg:
1. 
```
./vcpkg/vcpkg install yaml-cpp
```

# Build Instructions

Set Configuration to 'Debug' or 'Release' then build the solution (F7).

```
Build started at 10:05 PM...
1>------ Skipped Build: Project: ServerTestVSProj, Configuration: Debug x64 ------
1>Project not selected to build for this solution configuration 
2>------ Build started: Project: ServerVSProj, Configuration: Debug x64 ------
3>------ Build started: Project: ClientVSProj, Configuration: Debug x64 ------
2>Compile flatbuffer
2>Component.cpp
3>Component.cpp
2>Entity.cpp
3>Entity.cpp
2>KeyMap.cpp
3>KeyMap.cpp
...
========== Build: 2 succeeded, 0 failed, 0 up-to-date, 2 skipped ==========
========== Build completed at 10:06 PM and took 29.751 seconds ==========
```

# Run Instructions

Execute CLientVsProj.exe and ServerVSProj.exe, or run quick_start.bat to create a server and client.

```
start "client" "..\..\bin\client\x64\Debug\exe\ClientVsProj.exe"
start "server" "..\..\bin\server\x64\Debug\exe\ServerVsProj.exe" "--port" "35656" "--tick" "15"
```
