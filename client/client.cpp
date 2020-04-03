#include "client.h"
#include "../lib/room_info.h"
#include "../lib/tcp_utilities.h"
#include "../lib/send_command.h"

#include <iostream>
#include <vector>

volatile sig_atomic_t Client::exit_flag_ = 0;

Client::Client() {
  ConfigureSignals();
}

Client::~Client() {
  if (socket_ != -1) {
    shutdown(socket_, SHUT_RDWR);
    char buffer[4096];
    int ret;
    do {
      ret = read(socket_, buffer, sizeof(buffer));
    } while (ret > 0);
    close(socket_);
  }
}


// TODO: ERROR PARSING
void Client::Run(const char* ip_string, const int port) {
  try {
    interface_manager_.Start();
    std::string login = interface_manager_.AskLogin();

    //Connect(ip_string, port);

    std::vector<RoomInfo> rooms_info;

    RoomInfo room;
    room.id = 0;
    room.participant_num = 2;
    rooms_info.push_back(room);
    room.id = 1;
    room.participant_num = 100;
    rooms_info.push_back(room);

    while (1) {
      CommandToManager command = interface_manager_.MainServerPage(rooms_info);
      if (command == JOIN_ROOM) {
        int room_id = interface_manager_.GetRoomId();
      }

      while(1) {

      }

    }

  } catch (int err_code) {
    if (err_code == -1) {
      return;
    }
    throw std::current_exception();
  }
  return;

  std::vector<RoomInfo> rooms_info;
  uint32_t room_number;
  std::cout << "Prepare for read\n";
  try {
    read_uint32_by_char(socket_, &room_number);
  } catch(...) {
    perror("Server read error");
    return;
  }
  rooms_info.resize(room_number);
  std::cout << room_number << " LOL\n";

  std::vector<RoomInfo> rooms(room_number);
  for (int i = 0; i < room_number; ++i) {
    rooms[i].Read(socket_);
    std::cout << rooms[i].id << "\n";
  }
  int com = 0;
 // std::cin >> com; // 0 - CREATE_ROOM, 1 - JOIN_ROOM
  CommandToManager command;
  if (com == 0) {
    command = CREATE_ROOM;
  }


  if (command == JOIN_ROOM) {
    WriteCommand(CommandToManager::JOIN_ROOM, socket_);

    std::string login = "PussyDestroyerXXX";
    //std::cin >> login;

    uint32_t login_size = login.size();
    write_uint32(socket_, &login_size);
    write_buffer(socket_, login.data(), static_cast<size_t>(login_size));
  } else if (command == CREATE_ROOM) {
    WriteCommand(CommandToManager::CREATE_ROOM, socket_);
    std::string login;
    std::cin >> login;
    std::cout << "Lol";
    uint32_t login_size = login.size();
    write_uint32(socket_, &login_size);
    write_buffer(socket_, login.data(), static_cast<size_t>(login_size));
    while (1) {
      1==1;
    }
  }
  return;
}

void Client::ExitHandler(int signum) {
  exit_flag_ = 1;
}

void Client::ConfigureSignals() {
  struct sigaction action_int;
  memset(&action_int, 0, sizeof(action_int));
  action_int.sa_handler = Client::ExitHandler;
  int ret;
  ret = sigaction(SIGTERM, &action_int, NULL);
  if (ret < 0) {
    throw std::runtime_error("Error while sigaction()");
  }
  ret = sigaction(SIGINT, &action_int, NULL);
  if (ret < 0) {
    throw std::runtime_error("Error while sigaction()");
  }
}

void Client::Init() {
  socket_ = socket(AF_INET, SOCK_STREAM, 0);
  if (socket_ < 0) {
    throw std::runtime_error("Unable to bind socket");
  }
}

void Client::Connect(const char *ip_string, const int port) {
  struct in_addr server_in_addr;
  inet_aton(ip_string, &server_in_addr);

  struct sockaddr_in server_sockaddr;
  server_sockaddr.sin_family = AF_INET;
  server_sockaddr.sin_addr = server_in_addr;
  server_sockaddr.sin_port = htons(port);

  int ret = connect(socket_, (const struct sockaddr*)&server_sockaddr,
      sizeof(struct sockaddr_in));
  if (ret < 0) {
    throw std::runtime_error("connect() error");
  }
}
