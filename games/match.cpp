#include "games/match.h"
#include "gflags/gflags.h"

DEFINE_bool(log_games, false, "Should games be logged");
DEFINE_string(log_dir, "", "Directory to save log files to");

Match::Match() {
  if (FLAGS_log_games) {
    int timestamp = time(0);
    auto dir_with_slash = FLAGS_log_dir[FLAGS_log_dir.size() - 1] == '/' ?
                          FLAGS_log_dir : FLAGS_log_dir + "/";
    string filename = FLAGS_log_dir + "game_" + std::to_string(timestamp);
    printf("Saving game log to: %s\n", filename.c_str());
    log_file_ = fopen(filename.c_str(), "w");
    fprintf(log_file_, "[\n");
  }
}

Match::~Match() {
  if (log_file_ != nullptr) {
    fprintf(log_file_, "]\n");
    fclose(log_file_);
  }
}

void Match::Publish(const Message& message) {
  if (log_file_ != nullptr) {
    if (written_++ > 0) fprintf(log_file_, ",\n");
    fprintf(log_file_, "%s\n", message.ToString().c_str());
  }
}
