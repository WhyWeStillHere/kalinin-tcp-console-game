#include "../lib/game_settings.h"
#include "server.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[]) {
    char* ip_str = argv[1];
    int port = static_cast<int>(strtol(argv[2], NULL, 10));

    if (argc <= 2) {
      fprintf(stderr,"\nThis is game server.\n\n"
                     "We need host_name and port number in argumens of prorgam\n\n");
      return EXIT_FAILURE;
    }

    // Load settings from default file
    GetCurrentSettings()->LoadSettings();

    // Program can be terminated here
    GameServer server(PROGRAM);
    server.Init(ip_str, port);
    server.Run();
    return EXIT_SUCCESS;
}
