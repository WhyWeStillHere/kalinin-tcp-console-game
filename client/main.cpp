#include "client.h"

int main(int argc, char* argv[]) {
  if (argc < 2) {
    perror("To few args\n");
    return EXIT_FAILURE;
  }
  char* ip_str = argv[1];
  int port = static_cast<int>(strtol(argv[2], NULL, 10));

  Client client;
  client.Init();

  if (strcmp(ip_str, "localhost") == 0) {
    client.Run("127.0.0.1", port);
  } else {
    client.Run(ip_str, port);
  }
  return 0;
}