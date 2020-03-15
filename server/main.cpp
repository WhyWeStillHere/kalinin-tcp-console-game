#include <cstdio>
#include <cstdlib>
#include "server.h"

int main(int argc, char* argv[]) {
    char* ip_str = argv[1];
    int port = static_cast<int>(strtol(argv[2], NULL, 10));

    if (argc <= 2) {
      perror("To few args\n");
      return EXIT_FAILURE;
    }

    // Program can be terminated here
    GameServer server(DAEMON);
    server.Init(ip_str, port);
    server.Run();
    return EXIT_SUCCESS;
}