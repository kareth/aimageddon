#include "games/lobby.h"
#include "games/match.h"
#include "common/connection.h"

#include "common/declarations.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"

using testing::Return;
using testing::ByMove;

unique_ptr<Message> MakeJoinRequest(int num_players) {
  Json request;
  request["type"] = "join_game";
  Json data;
  data["num_players"] = num_players;
  request["data"] = data;
  return unique_ptr<Message>(new Message(request));
}

enum GameStatus {
  kStarted,
  kQueued
};

class MatchMock : public Match {
 public:
  MatchMock(int expected_players) : Match(expected_players) {}
  virtual ~MatchMock() {}

  virtual bool CheckOptionsCompatibility(const Json& match_options) override{
    return num_players_ == match_options.get("num_players", Json(0)).as_int();
  }

  virtual void StartGame(std::function<void()> finish_callback) {
    StartGameInternal();
    finish_callback();
  }

  MOCK_METHOD0(StartGameInternal, void());
};

class MatchFactoryMock : public MatchFactory {
 public:
  ~MatchFactoryMock() {}
  MOCK_METHOD1(CreateMatch, unique_ptr<Match>(const Json& json));
};

class FakeConnection : public Connection {
 public:
  FakeConnection(int num_players) : num_players_(num_players) {}
  ~FakeConnection() {}

  virtual void RegisterForMessage(Callback callback) override {
    callback(MakeJoinRequest(num_players_));
  };

  virtual void Write(const Message& message) override { return; }
  virtual unique_ptr<Message> Read() override { return nullptr; }
  virtual bool active() override { return true; }

 private:
  int num_players_;
};

MATCHER_P(SameNumConnections, num_players, "") {
  return num_players == arg.get("num_players", Json(0)).as_int();
}

class SequentialLobbyTest : public testing::Test {
 public:
  void SetUp() {
    unique_ptr<MatchFactoryMock> match_factory(new MatchFactoryMock());
    match_factory_ = match_factory.get();
    lobby_.reset(new SequentialLobby(unique_ptr<MatchFactory>(match_factory.release())));
  }

  void SetMatchExpectation(int num_players, GameStatus status) {
    unique_ptr<MatchMock> match(new MatchMock(num_players));
    if (status == kStarted) {
      EXPECT_CALL(*match, StartGameInternal()).Times(1);
    }
    EXPECT_CALL(*match_factory_, CreateMatch(SameNumConnections(num_players)))
      .WillOnce(Return(ByMove(unique_ptr<Match>(match.release()))));
  }

  void JoinGame(int num_players) {
    unique_ptr<Connection> player(new FakeConnection(num_players));
    lobby_->AddPlayer(std::move(player));
  }

 private:
  unique_ptr<Lobby> lobby_;
  MatchFactoryMock* match_factory_;
};

TEST_F(SequentialLobbyTest, SingleSoloRequest) {
  SetMatchExpectation(1, kStarted);
  JoinGame(1);
}

TEST_F(SequentialLobbyTest, TwoSoloRequests) {
  SetMatchExpectation(1, kStarted);
  JoinGame(1);
  SetMatchExpectation(1, kStarted);
  JoinGame(1);
}

TEST_F(SequentialLobbyTest, StartedDoubleGame) {
  SetMatchExpectation(2, kStarted);
  JoinGame(2);
  JoinGame(2);
}

TEST_F(SequentialLobbyTest, QueuedDoubleGame) {
  SetMatchExpectation(2, kQueued);
  JoinGame(2);
}

TEST_F(SequentialLobbyTest, TwoGames) {
  SetMatchExpectation(1, kStarted);
  SetMatchExpectation(2, kStarted);
  JoinGame(2);
  JoinGame(1);
  JoinGame(2);
}

TEST_F(SequentialLobbyTest, DifferentStateGames) {
  SetMatchExpectation(1, kStarted);
  SetMatchExpectation(2, kQueued);
  SetMatchExpectation(3, kStarted);
  JoinGame(2);
  JoinGame(3);
  JoinGame(1);
  JoinGame(3);
  JoinGame(3);
}

TEST_F(SequentialLobbyTest, MultipleWaitingGames) {
  SetMatchExpectation(2, kQueued);
  SetMatchExpectation(3, kQueued);
  SetMatchExpectation(4, kQueued);
  JoinGame(2);
  JoinGame(3);
  JoinGame(3);
  JoinGame(4);
  JoinGame(4);
  JoinGame(4);
}
