# ProjectM

ProjectM is a 2D multiplayer game.

Directory structure is as follows:
* The [client/](client/) folder has the Client code
* The [server/](server/) folder has the Server code
* The [tests/](tests/) folder has unit tests for both client and server
* The [common/](common/) folder has shared code used by both Client and Server
* The [lib/](lib/) folder has third party dependencies

Note: The client, server, and tests folders have their own Visual Studio ``.vcxproj`` 'Project' file(s), under the [``ProjectM.sln``](ProjectM.sln) 'Solution' file in the root directory.

Tasks are tracked on a [Trello Board](https://trello.com/invite/b/cWteNm74/bf64646388becf6430bf7d5b8bd4df55/projectm).

# Build Instructions


1. Clone this project and update submodules:

```
git clone https://github.com/evaaan/ProjectM.git
cd ProjectM
git submodule init
git submodule update
```

2. Change to the [lib/](lib/) directory:

```
cd lib
```

2. Install [Microsoft/vcpkg](https://github.com/Microsoft/vcpkg) with VS integration:

```
./vcpkg/bootstrap-vcpkg.bat
./vcpkg/vcpkg integrate install
```

3. Install [ValveSoftware/GameNetworkingSockets](https://github.com/ValveSoftware/GameNetworkingSockets):

```
./vcpkg/vcpkg --overlay-ports=./GameNetworkingSockets/vcpkg_ports install gamenetworkingsockets
```

4. Install [Google FlatBuffers](https://google.github.io/flatbuffers/index.html):

```
./vcpkg/vcpkg install flatbuffers
```

If everything worked, Visual Studio project should recognize header includes for all vcpkg-installed projects (under [/lib/vcpkg/installed/x86-windows/include](/lib/vcpkg/installed/x86-windows/include). If it didn't work, add the sub-directories of that path to the Additional Directories path in Visual Studio. The following should be recognized by VS intellisense:

```
#include <steam/steamnetworkingsockets.h>
```
