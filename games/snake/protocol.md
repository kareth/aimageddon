SNAKE PROTOCOL

At the start of the game a single message is sent to all plaers.
The message contains description of the game that is about to begin.
GAME_START MESSAGE:
{
  type: game_start
  data: {
    general: {
      action_time: 400
      turns: 100
      turns_to_starve: 0
    }
    board: {
      dimensions: { x: 5, y: 10 }
    }
    players: [
      {
        name: "player_name"
        placement: [{x:5, y:10}, {...}, {...}]
       }
       ...
    ]
    apples: [{x:1, y:2}, ...]
  }
}
Note: move_time is given in milliseconds.
if turns_to_starveis set to 0, its disabled

The message above initiates a match, and players are expected to
send their action after receiving it, formed in the following way:
ACTION MESSAGE:
{
  type: action
  turn: 3
  data: {
    direction: left/right/up/down
  }
}
The first action message should have turn equals 1, following should be +1
If there is no message received or the direction field is null/empty/invalid
no action is taken and snake continues to move in previous direction.

After each turn server sends update about the board. In order to reduce amount
of data being sent, only current head positions are sent, along with events.
GAME STATUS MESSAGE:
{
  type: game_status
  turn: 3
  data: {
    positions: [
      {x:5, y:7},
      ...
    ]
    events: [
      {
        type: apple_spawn (or apple_despawn)
        position: {x:5, y:5}
      }, {
        type: death
        player_name: payername
      }, {
        type: apple_consumption
        player_name: playername
      }
    ]
  }
}
The turn index sent is the message equals to the turn that has just finished.

At the end, a simple ending message is sent, along with final scores.
GAME_END MESSAGE:
{
  type: game_end
  data: {
    winner: player_name
    scores: [
      {
        player_name: name
        max_length: 102
        deaths: 0
       }
    ]
  }
}
