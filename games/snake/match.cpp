#include "games/snake/match.h"

#include "gflags/gflags.h"
#include <unistd.h>

DEFINE_bool(log_games, false, "Should games be logged");
DEFINE_string(log_dir, "", "Directory to save log files to");

//////////////////
// Factory
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
int SnakeMatch::Options::ParseFromJson(const Json& json) {
  num_players = json.get("num_players", Json(0)).as_int();
  if (num_players < 1 || num_players > 4) return 1;
  return 0;
}

bool SnakeMatch::Options::IsCompatible(const Options& rhs) {
  return num_players == rhs.num_players;
}

//////////////////
// Helper functions

namespace {
Json MakeBase(const string& message_type, const Json& data) {
  Json res;
  res["type"] = message_type;
  res["data"] = data;
  return res;
}

// TODO(pzu) move to some player wrapper
int TurnOf(const Message& msg) {
  return msg.content().get("turn", Json(0)).as_int();
}

Json PointToJson(const Point& point) {
  Json json;
  json["x"] = point.x;
  json["y"] = point.y;
  return json;
}

string PlayerName(int index) {
  string name = "player";
  name += char('0' + index);
  return name;
}

Json MakeDeath(const string& player_name) {
  Json res;
  res["type"] = "death";
  res["player_name"] = player_name;
  return res;
}

}  // namespace

//////////////////
// Match
SnakeMatch::SnakeMatch(const Options& options)
  : RoundBasedMatch(options.num_players), options_(options) {
    board_(options.x, options.y, kEmpty),
    snakes_(options.num_players) {
  printf("New match initiated.\n");
}

bool SnakeMatch::CheckOptionsCompatibility(const Json& match_params) {
  SnakeMatch::Options opts;
  if (opts.ParseFromJson(match_params)) return false;

  return options_.IsCompatible(opts);
}

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
    player_info["name"] = PlayerName(p);
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
  //for (auto& event : events_)

  Json data;
  data["positions"] = positions;
  data["events"] = events;
  Json base = MakeBase("game_status", data);
  base["turn"] = current_turn_;
  return Message(base);
}

SnakeMatch::Action SnakeMatch::ParseAction(const Message& message) {
  Json data = message.content().get("data", Json(""));
  string direction = data.get("direction", "").as_string();
  if (direction == "left") return kLeft;
  if (direction == "right") return kRight;
  return kForward;
}

void SnakeMatch::SpawnSnakes() {
  for (int p = 0; p < num_players_; p++) {
    for (int i = 0; i < options_.starting_length; i++)
      snakes_[p].points.push_front(Point(p + 1, i + 1));
    snakes_[p].direction = Point(1, 0);
  }

  // TODO(multiple snakes);
  /*int x = options_.x, y = options_.y;
  for (int p = 0; p < num_players_; p++) {
    for (int i = 0; i < snake.size(); i++) {
      Point field = snake[i] * modifiers[p] + bases[p];
      snakes_[p].points.push_front(field);
      board_[field] = p;
    }
    snakes_[p].direction = snake[1] * modifiers[p] - snake[0] * modifiers[p];
  }*/
}

vector<SnakeMatch::Action> SnakeMatch::FetchActions() {
  vector<Action> actions;
  for (auto& player : players_) {
    auto msg = player->Read();
    Action action = Action::kForward;
    while (msg != nullptr) {
      if (msg->type() == "action" &&
          (TurnOf(*msg) == 0 || TurnOf(*msg) == current_turn_)) {
        action = ParseAction(*msg);
        break;
      }
      msg = player->Read();
    }
    actions.push_back(action);
  }
  return actions;
}

void SnakeMatch::ProcessTurn(const vector<Action>& actions) {
  for (int p = 0; p < num_players_; p++) {
    auto& s = snakes_[p];
    if (s.dead) continue;
    if (actions[p] == kLeft) s.direction.RotateLeft();
    if (actions[p] == kRight) s.direction.RotateRight();
  }
  for (int p = 0; p < num_players_; p++) {
    auto& s = snakes_[p];
    if (s.dead) continue;
    board_[s.points.back()] = kEmpty;
    s.points.pop_back();
  }
  for (int p = 0; p < num_players_; p++) {
    auto& s = snakes_[p];
    if (s.dead) continue;
    Point new_head = s.points.front() + s.direction;
    if (!board_.Inside(new_head)) {
      events_.push_back(MakeDeath(PlayerName(p)));
      s.dead = true;
    } if (board_[new_head] == kApple) {
    } else if (board_[new_head] != kEmpty) {
      events_.push_back(MakeDeath(PlayerName(p)));
      s.dead = true;
    }
    s.points.push_front(new_head);
  }
  current_turn_++;
}

bool SnakeMatch::TestEndGame() {
  for (auto& s : snakes_)
    if (!s.dead) return false;
  return true;
}

void SnakeMatch::StartGame(std::function<void()> finish_callback) {
  printf("Starting a game.\n");
  vector<Message> log;
  SpawnSnakes();

  Broadcast(Record(MakeGameStart()));
  for (int turn = 0; turn < options_.max_turns; turn++) {
    usleep(1000 * options_.action_time);
    auto actions = FetchActions();
    ProcessTurn(actions);
    if (TestEndGame()) break;
    auto game_status = Record(MakeGameStatus());
    if (turn != options_.max_turns - 1)
      Broadcast(game_status);
  }

  Broadcast(Record(MakeGameEnd()));

  if (FLAGS_log_games) {
    int timestamp = time(0);
    string filename = FLAGS_log_dir + "game_" + std::to_string(timestamp);
    FILE* file = fopen(filename.c_str(), "w");
    fprintf(file, "[\n");
    for (int i = 0; i < game_log_.size(); i++) {
      if (i != 0) fprintf(file, ",");
      fprintf(file, "%s", game_log_[i].ToString().c_str());
    }
    fprintf(file, "]\n");
    printf("Saved game log to file: %s\n", filename.c_str());
    fclose(file);
  }

  finish_callback();
}
