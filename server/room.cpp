#include "room.h"
#include "../lib/network_utilities/send_command.h"
#include <cstring>
#include <sys/socket.h>
#include <sys/syslog.h>

volatile sig_atomic_t Room::signal_exit_flag_ = 0;

Room::Room(const int server_fd, const int creator_fd)
    : server_fd_(server_fd), creator_fd_(creator_fd) {
  ConfigureSignals();
  state_ = WAITING;
}

void Room::ExitHandler(int signum) {
  signal_exit_flag_ = 1;
}

Room::~Room() {
  while (!connections_.empty()) {
    CloseConnection((*connections_.begin()).first);
  }
  //CloseConnection(creator_fd_);
  if (server_fd_ != -1) {
    char buffer[4096];
    int ret;
    do {
      ret = read(server_fd_, buffer, sizeof(buffer));
    } while (ret > 0);
    shutdown(server_fd_, SHUT_RDWR);
    close(server_fd_);
    syslog (LOG_NOTICE, "Server fd correctly closed in room");
  }
  syslog (LOG_NOTICE, "Room terminated");
}

void Room::CloseConnection(const int fd) {
  std::cout << fd << " Deletion\n";
  io_context_.DelEvent(fd);
  shutdown(fd, SHUT_RDWR);
  connections_.erase(fd);
  char buffer[4096];
  int ret;
  do {
    ret = read(fd, buffer, sizeof(buffer));
  } while (ret > 0);
  close(fd);
  syslog (LOG_NOTICE, "Connection closed");
}

void Room::ConfigureSignals() {
  struct sigaction action_int;
  memset(&action_int, 0, sizeof(action_int));
  action_int.sa_handler = Room::ExitHandler;
  int ret;
  /*ret = sigaction(SIGTERM, &action_int, NULL);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while sigaction(): %s", strerror(errno));
  }*/
  ret = sigaction(SIGINT, &action_int, NULL);
  if (ret < 0) {
    syslog(LOG_NOTICE, "Error while sigaction(): %s", strerror(errno));
  }
}

void Room::Run() {
  io_context_.AddEvent(READ_FD, SERVER, server_fd_);

  TcpConnection<PlayerStates> nw_connection;
  nw_connection.state = ASKING_LOGIN_SIZE;
  connections_[creator_fd_] = nw_connection;
  io_context_.AddEvent(WRITE_FD, CREATOR, creator_fd_);
  epoll_event* events;
  std::cout << signal_exit_flag_ << " " << exit_flag_ << "\n";
  while (!signal_exit_flag_ && !exit_flag_) {
    try {
      std::cout << "Waiting\n";
      int event_num = io_context_.Wait(events);
      //syslog (LOG_WARNING, "Room events %d", event_num);
      std::cout << event_num << " Events\n";
      if (state_ != PLAYING) {
        ManageEvents(event_num, events);
      } else {
        ManageGame(event_num, events);
      }
    } catch (const std::exception& err) {
      syslog (LOG_WARNING, "Error in room runtime: %s", err.what());
    }
  }
  syslog (LOG_NOTICE, "Room stopped");
}

void Room::RemovePlayer(const int fd) {
  players_.erase(fd);
  CloseConnection(fd);
  UpdatePlayerList();
}

void Room::ManageEvents(const int event_num, epoll_event* events) {
  for (size_t i = 0; i < event_num; ++i) {
    epoll_event* event = &events[i];
    auto data = static_cast<IOContext::data_t *>(event->data.ptr);
/*
    if (data->fd_type == CREATOR) {
      syslog (LOG_WARNING, "Creator event");
      if (event->events & EPOLLIN) {
        syslog (LOG_WARNING, "Creator read");
        char buffer[4];
        int ret = read(data->fd, buffer, sizeof(buffer));
        syslog (LOG_WARNING, "Creator read: %d %s", ret, buffer);
      }
      if (event->events & EPOLLOUT) {
        syslog (LOG_WARNING, "Creator write");
      }
    }*/

    if (event->events & (EPOLLHUP | EPOLLERR | EPOLLPRI)) {
      if (data->fd_type == CREATOR) {
        syslog (LOG_WARNING, "Host error");
        signal_exit_flag_ = 1;
        return;
      }
      CloseConnection(data->fd);
      continue;
    }

    if (data->fd_type == CLIENT) {
      if (connections_[data->fd].state == READY_TO_PLAY
          && (event->events & EPOLLIN)) {
        syslog (LOG_WARNING, "Player exit");
        RemovePlayer(data->fd);
      }
      ManagePlayer(data->fd);
    }
    if (data->fd_type == CREATOR) {
      try {
        if (connections_[data->fd].state != READY_TO_PLAY) {
          ManagePlayer(data->fd);
        } else {
          ManageHost(data->fd);
        }
      } catch(...) {
        exit_flag_ = true;
        return;
      }
    }
    if (data->fd_type == SERVER) {
      try {
        syslog (LOG_WARNING, "Receiving connection to room");
        int player_fd;
        int ret = read(data->fd, &player_fd, sizeof(int));
        AddPlayer(player_fd);
      } catch(...) {
        syslog (LOG_WARNING, "Error while room connection");
        std::rethrow_exception(std::current_exception());
      }
    }
  }
}

void Room::ManageGame(int event_num, epoll_event* events) {
  for (size_t i = 0; i < event_num; ++i) {
    epoll_event* event = &events[i];
    auto data = static_cast<IOContext::data_t *>(event->data.ptr);

    if (event->events & (EPOLLHUP | EPOLLERR | EPOLLPRI)) {
      if (data->fd_type == CREATOR) {
        syslog (LOG_WARNING, "Host error");
        signal_exit_flag_ = 1;
        return;
      }
      CloseConnection(data->fd);
      continue;
    }
    if (data->fd_type == CLIENT || data->fd_type == CREATOR) {
        ManagePlayerEvent(data->fd);
    }
    if (data->fd_type == TIMER) {
        ApplyGameChanges(data->fd);
    }
  }
}

void Room::ManagePlayerEvent(const int fd) {
  int player_id = players_[fd].id;
  auto command(ReadCommand<CommandToGame>(fd));
  switch (command) {
  case MOVE_UP:
    game_.MovePlayer(player_id, UP);
    break;
  case MOVE_DOWN:
    game_.MovePlayer(player_id, DOWN);
    break;
  case MOVE_LEFT:
    game_.MovePlayer(player_id, LEFT);
    break;
  case MOVE_RIGHT:
    game_.MovePlayer(player_id, RIGHT);
    break;
  case SHOOT:
    game_.ShootProjectile(player_id);
    break;
  }

}

void Room::UpdateGameInfo() {
  GameInfo game_info = game_.GetInfo();
  for (auto& connection: connections_) {
    PlayerStates& player_state = connection.second.state;
    PlayerStates& key_state = connection.second.key_state;
    std::vector<char>& buffer = connection.second.buffer;
    size_t& index = connection.second.operation_index;
    game_info.WriteBuffer(buffer);
  }
}

void Room::SendBuffers() {
  for (auto&  connection: connections_) {
    std::vector<char>& buffer = connection.second.buffer;
    if (buffer.empty()) {
      return;
    }
    size_t& index = connection.second.operation_index;
    char* buf = buffer.data();
    index += write_buffer_some(connection.first,
                               buf + static_cast<int>(index),
                               buffer.size() * sizeof(char));
    if (index == buffer.size()) {
      buffer.clear();
      index = 0;
    }
  }
}

void Room::AddPlayer(const int fd) {
  syslog (LOG_WARNING, "Adding new player");
  std::cout << fd << " player\n";
  TcpConnection<PlayerStates> nw_connection;
  nw_connection.state = ASKING_LOGIN_SIZE;
  nw_connection.key_state = READY_TO_PLAY;
  connections_[fd] = nw_connection;
  players_[fd].id = fd;
  io_context_.AddEvent(READ_FD, CLIENT, fd);
  ++waiting_persons_;
}

void Room::UpdatePlayerList() {
  for (auto& connection: connections_) {
    if (connection.second.state == READY_TO_PLAY) {
      std::vector<char>& buffer = connection.second.buffer;
      WriteBufferCommand<CommandToPlayer>(UPDATE_PLAYERS, buffer);
      connection.second.state = WRITE_PLAYERS_NUM;
      io_context_.ChangeEvent(WRITE_FD, connection.first);
    }
  }
}

void Room::ManageHost(const int fd) {
  PlayerStates& player_state = connections_[fd].state;
  std::vector<char>& buffer = connections_[fd].buffer;
  size_t& index = connections_[fd].operation_index;
  auto command = ReadCommand<CommandToRoom>(fd);
  if (command == START_GAME_HOST) {
    state_ = STARTING;
    player_state = STARTING_TO_PLAY;
    io_context_.DelEvent(server_fd_);
    shutdown(server_fd_, SHUT_RDWR);
    close(server_fd_);
    syslog (LOG_WARNING, "Starting game");
    for (auto& connection: connections_) {
      if (connection.first != creator_fd_) {
        if (connection.second.state == READY_TO_PLAY) {
          connection.second.state = STARTING_TO_PLAY;
          io_context_.ChangeEvent(WRITE_FD, connection.first);
        } else {
          connection.second.key_state = STARTING_TO_PLAY;
        }
      } else {
        connection.second.state = STARTING_TO_PLAY;
      }
    }
  }
}

void Room::ManagePlayer(const int fd) {
  PlayerStates& player_state = connections_[fd].state;
  PlayerStates& key_state = connections_[fd].key_state;
  std::vector<char>& buffer = connections_[fd].buffer;
  size_t& index = connections_[fd].operation_index;
  if (player_state == ASKING_LOGIN_SIZE) {
    uint32_t login_size;
    read_uint32(fd, &login_size);
    buffer.resize(login_size);
    player_state = ASKING_LOGIN;
  }
  if (player_state == ASKING_LOGIN) {
    index += read_buffer_some(fd, buffer.data() + index, buffer.size());
    if (index == buffer.size()) {
      std::string s(buffer.begin(), buffer.end());
      players_[fd].login = std::move(s);
      syslog (LOG_WARNING, "New login: %s", buffer.data());
      buffer.clear();
      index = 0;
      player_state = WRITE_PLAYERS_NUM;
      io_context_.ChangeEvent(WRITE_FD, fd);
      if (fd == creator_fd_) {
        host_.login = players_[fd].login;
      }
      UpdatePlayerList();
    }
  }
  if (player_state == WRITE_PLAYERS_NUM) {
    std::vector<PlayerInfo> vect;
    vect.push_back(host_);
    for (auto& player_info: players_) {
      if (connections_[player_info.first].state != ASKING_LOGIN &&
          connections_[player_info.first].state != ASKING_LOGIN_SIZE &&
          player_info.first != creator_fd_) {
        vect.push_back(player_info.second);
      }
    }
    syslog (LOG_WARNING, "Player num: %d", vect.size());
    write_vector<PlayerInfo>(buffer, vect);
    player_state = WRITE_PLAYERS;
  }
  if (player_state == WRITE_PLAYERS) {
    char* buf = buffer.data();
    index += write_buffer_some(fd,
                               buf + static_cast<int>(index),
                               buffer.size() * sizeof(char));
    if (index == buffer.size()) {
      buffer.clear();
      index = 0;
      if (key_state == STARTING_TO_PLAY) {
        player_state = STARTING_TO_PLAY;
        io_context_.DelEvent(fd);
      } else {
        player_state = READY_TO_PLAY;
        io_context_.ChangeEvent(READ_FD, fd);

        syslog (LOG_WARNING, "Everething is OK");
        return;
      }
    }
  }
  if (player_state == READY_TO_PLAY) {
    CloseConnection(fd);
    return;
  }
  if (player_state == STARTING_TO_PLAY) {
    WriteCommand<CommandToPlayer>(CommandToPlayer::START_GAME, fd);
    --waiting_persons_;
    if (waiting_persons_ == 0) {
      StartGame();
    }
  }
}

void Room::StartGame() {
  state_ = PLAYING;
  std::vector<PlayerInfo> players;
  for (const auto& player: players_) {
    if (player.first != host_.id) {
      players.emplace_back(player.second);
    }
  }

  game_.Init(players);

  // Preparing to read
  for (const auto& connection: connections_) {
    io_context_.ChangeEvent(READ_FD, connection.first);
  }
  UpdateGameInfo();
  SendBuffers();

  io_context_.SetTimer(game_.GetUpdateWaitTime());
}

void Room::ApplyGameChanges(const int timer_fd) {
  uint64_t timer_alarms;
  read(timer_fd, &timer_alarms, sizeof(timer_alarms));
  game_.UpdateGame(timer_alarms);
  UpdateGameInfo();
  SendBuffers();
}