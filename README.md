# The Labyrinth
The Labyrinth is a terminal multiplayer game with network communication features

## Requirements 
* cmake version >= 3.13
## Installation
```
cd $PRODJECT_FOLDER_PATH
cmake ./CMakeLists.txt 
make
```

## Launch
### Server launch
Server binary placed in server folder

You can adjust game settings, by changing server_settings file (More info below)

To launch server use command
```
./server [ip address] [port]
```
Server will accept connections on this address and port

Default: ip address = 127.0.0.1 port = 9091

On default server starts as program

### Client launch
Client binary placed in server folder
To launch clien use command
```
./client [ip address] [port]
```
Client will connect to this address and port
Default: ip address = 127.0.0.1 port = 9091
Game will starts in your current terminal window
