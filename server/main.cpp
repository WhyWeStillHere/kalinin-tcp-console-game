#include "../lib/game_settings.h"
#include "server.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char* argv[]) {
  int port = 9091;
  ServerStartUpType type = PROGRAM;
  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "daemon") == 0) {
      type = DAEMON;
    } else {
      port = static_cast<int>(strtol(argv[i], NULL, 10));
    }
  }

  // Load settings from default file
  GetCurrentSettings()->LoadSettings();

  // Program can be terminated here
  GameServer server(type);
  server.Init("127.0.0.1", port);
  server.Run();
  return EXIT_SUCCESS;
}
