#include <iostream>
#include <memory>
#include "ServerEngine.h"


int main()
{
    auto server = std::make_unique<ServerEngine>();
    server->Init();
    server->Run();
    server->Shutdown();
}
