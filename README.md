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

## Game description
Then you write your login, you can do 3 things:
- Refresh current list of available rooms
- Create your own room
- Join to available room

Then you in room list of current players updates automatically

### Create room
Then you create room you become a host of this room and you can start game, then you want it, 
and there are a least 2 players in the room
### Join room
Then you joining a room, you wait until host decided to start the game

### Game
Your main purpose to be last to die in this game
##### Controls
- You can move your player with WASD
- And shoot bullets with K key

##### Shooting
When you shoot with bullet, bullet choose closes target and bullet persue target until bullet collide with player

You lose, when your HP <= 0. Also, the you stay you hp increasing every second
