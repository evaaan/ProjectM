#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include "ServerEngine.h"

const int DEFAULT_SERVER_PORT = 35656;
const int DEFAULT_TICK_MS = 50;

void PrintUsageAndExit()
{
    fflush(stderr);
    printf(
        R"usage(Usage:
    ServerVSProj.exe [--port PORT] [--tick TICK]
)usage"
    );
    fflush(stdout);
    exit(1);
}

/* Entry point */
int main(int argc, char *argv[])
{
    // default
    int port_num = 35656;
    int tick_ms = 50;

    // parse arguments
    for (int i = 1; i < argc; ++i)
    {
        // must specify --port
        if (!strcmp(argv[i], "--port"))
        {
            ++i;
            if (i >= argc)
                PrintUsageAndExit();
            port_num = atoi(argv[i]);
            if (port_num <= 0 || port_num > 65535)
            {
                printf("Invalid port %d\n", port_num);
                PrintUsageAndExit();
            }
            continue;
        }
        // must specify --tick
        if (!strcmp(argv[i], "--tick"))
        {
            ++i;
            if (i >= argc)
                PrintUsageAndExit();
            tick_ms = atoi(argv[i]);
            if (tick_ms < 0)
            {
                printf("Invalid tick ( %d )ms\n", tick_ms);
                PrintUsageAndExit();
            }
            continue;
        }
        // help
        if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h"))
        {
            PrintUsageAndExit();
        }
    }
    printf("Creating ServerEngine..\n");
    auto server = std::make_unique<ServerEngine>(port_num, tick_ms);
    printf("Running ServerEngine..\n");
    server->Run();
    printf("Shutting down.\n");
    server->Shutdown();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}
