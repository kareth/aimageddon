#include "games/snake/match.h"

//////////////////
// Factory
//
SnakeMatchFactory::SnakeMatchFactory() {
}

unique_ptr<Match> SnakeMatchFactory::CreateMatch(
    const Json& match_params) {
  SnakeMatch::Options opts;
  if (opts.ParseFromJson(match_params))
    return nullptr;

  return unique_ptr<Match>(new SnakeMatch(opts));
}

//////////////////
// Options
//
int SnakeMatch::Options::ParseFromJson(const Json& json) {
  num_players = json.get("num_players", Json(0)).as_int();
  if (num_players < 1 || num_players > 4) return 1;
  return 0;
}

bool SnakeMatch::Options::IsCompatible(const Options& rhs) {
  return num_players == rhs.num_players;
}

//////////////////
// Match
//
namespace {
Json MakeDeath(const int player_id) {
  Json res;
  res["type"] = "death";
  res["player"] = player_id;
  return res;
}

SnakeMatch::Action ParseAction(const Message& message) {
  Json data = message.content().get("data", Json(""));
  string direction = data.get("direction", "").as_string();
  if (direction == "left") return SnakeMatch::kLeft;
  if (direction == "right") return SnakeMatch::kRight;
  return SnakeMatch::kForward;
}

vector<SnakeMatch::Action> ParseActions(vector<unique_ptr<Message>> msgs) {
  vector<SnakeMatch::Action> actions;
  for (auto& msg : msgs) {
    if (msg == nullptr) {
      actions.push_back(SnakeMatch::kForward);
    } else {
      actions.push_back(ParseAction(*msg));
    }
  }
  return actions;
}
}  // namespace

SnakeMatch::SnakeMatch(const Options& options)
  : RoundBasedMatch(options.num_players), options_(options),
    board_(options.x, options.y, kEmpty) {
  printf("New match initiated.\n");
}

bool SnakeMatch::CheckOptionsCompatibility(const Json& match_params) {
  SnakeMatch::Options opts;
  if (opts.ParseFromJson(match_params)) return false;

  return options_.IsCompatible(opts);
}

void SnakeMatch::SpawnSnakes() {
  int x = options_.x, y = options_.y;
  vector<Point> starting_positions = {{1, 1}, {x-1, y-1}, {x-1, 1}, {1, y-1}};
  vector<Point> directions = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}};

  snakes_.clear();
  for (int p = 0; p < num_players_; p++) {
    snakes_.push_back(Snake());
    auto& s = snakes_.back();

    s.points.push_back(starting_positions[p]);
    s.direction = directions[p];
    s.id = p;

    for (int i = 0; i < options_.starting_length - 1; i++)
      s.points.push_front(s.points.front() + s.direction);
  }

  for (auto& s : snakes_) {
    for (auto& p : s.points) {
      board_[p] = s.id;
    }
  }
}


bool SnakeMatch::IsGameFinished() {
  for (auto& s : snakes_)
    if (!s.dead) return false;
  return true;
}

void SnakeMatch::AppleEaten(int snake_id) {
  // TODO(pzk) apple
}

void SnakeMatch::KillSnake(int snake_id) {
  snakes_[snake_id].dead = true;
  for (auto& p : snakes_[snake_id].points)
    if (board_.Inside(p) && board_[p] != kApple)
      board_[p] = kEmpty;
  events_.push_back(MakeDeath(snake_id));
}

void SnakeMatch::ProcessTurn(const vector<Action>& actions) {
  events_.clear();
  // Execute actions
  for (int p = 0; p < num_players_; p++) {
    auto& s = snakes_[p];
    if (s.dead) continue;
    if (actions[p] == kLeft) s.direction.RotateLeft();
    if (actions[p] == kRight) s.direction.RotateRight();
  }

  // Process moves
  for (auto& s : snakes_) {
    if (s.dead) continue;
    board_[s.points.back()] = kEmpty;
    s.points.push_front(s.points.front() + s.direction);
    s.ShortenTail();
  }

  // Check for collisions
  for (int p = 0; p < num_players_; p++) {
    auto& s = snakes_[p];
    if (s.dead) continue;

    auto& head = s.points.front();
    if (!board_.Inside(head) ||
        (board_[head] != kApple && board_[head] != kEmpty)) {
      KillSnake(p);
    }

    // Check for equal heads
    vector<int> same_heads;
    for (int h = 0; h < num_players_; h++)
      if (!snakes_[h].dead && snakes_[h].points.front() == head)
        same_heads.push_back(h);

    if (same_heads.size() > 1)
      for (int h : same_heads)
        KillSnake(h);
  }

  // Check for apples
  for (int p = 0; p < num_players_; p++) {
    auto& s = snakes_[p];
    if (s.dead) continue;
    if (board_[s.points.front()] == kApple) {
      s.RestoreTail();
      AppleEaten(p);
    }
  }

  // Mark fields on head as visited
  for (auto& s : snakes_)
    if (!s.dead)
      board_[s.points.front()] = s.id;

  current_turn_++;
}

void SnakeMatch::StartGame(std::function<void()> finish_callback) {
  printf("Starting a game.\n");
  SpawnSnakes();

  auto start = MakeGameStart();
  Broadcast(start);
  Log(start);

  for (int turn = 1; turn <= options_.max_turns && !IsGameFinished(); turn++) {
    auto actions = ParseActions(
        WaitForMessages(turn, std::chrono::milliseconds(options_.action_time)));

    ProcessTurn(actions);

    auto game_status = MakeGameStatus();
    Log(game_status);
    if (!IsGameFinished() && turn != options_.max_turns)
      Broadcast(game_status);
  }
  auto end = MakeGameEnd();
  Broadcast(end);
  Log(end);

  finish_callback();
}

/////////////////////////
// Messages constructors

namespace {
Json MakeBase(const string& message_type, const Json& data) {
  Json res;
  res["type"] = message_type;
  res["data"] = data;
  return res;
}
Json PointToJson(const Point& point) {
  Json json;
  json["x"] = point.x;
  json["y"] = point.y;
  return json;
}
}  // namespace

Message SnakeMatch::MakeGameStart() {
  Json general;
  general["action_time"] = options_.action_time;
  general["turns"] = options_.max_turns;
  general["turns_to_starve"] = options_.turns_to_starve;

  Json board;
  board["dimensions"] = PointToJson({options_.x, options_.y});

  Json players = Json::make_array();
  for (int p = 0; p < num_players_; p++) {
    Json player_info;
    player_info["name"] = "player" + std::to_string(p);
    player_info["placement"] = Json::make_array();
    for (int i = snakes_[p].points.size() - 1; i >= 0; i--)
      player_info["placement"].add(PointToJson(snakes_[p].points[i]));
    players.add(player_info);
  }

  Json apples = Json::make_array();
  for (auto& apple : apples_)
    apples.add(PointToJson(apple));

  Json data;
  data["general"] = general;
  data["board"] = board;
  data["players"] = players;
  data["apples"] = apples;

  return Message(MakeBase("game_start", data));
}

Message SnakeMatch::MakeGameEnd() {
  // TODO(pzk) fill results
  return Message(MakeBase("game_end", Json()));
}

Message SnakeMatch::MakeGameStatus() {
  Json positions = Json::make_array();
  for (int p = 0; p < num_players_; p++)
    positions.add(PointToJson(snakes_[p].points.front()));

  Json events = Json::make_array();
  for (auto& event : events_) {
    events.add(event);
  }

  Json data;
  data["positions"] = positions;
  data["events"] = events;
  Json base = MakeBase("game_status", data);
  base["round"] = current_turn_;
  return Message(base);
}
