#ifndef COMMUNICATION_MESSAGE_H_
#define COMMUNICATION_MESSAGE_H_

#include "common/declarations.h"
#include "jsoncons/json.hpp"

class Message {
 public:
  explicit Message(const Json& content);

  // REQUIRES: content needs to be parsable to json.
  explicit Message(const string& content);

  // Converts a message to string. Ensures a newline character at the end.
  string ToString() const;

  const Json& content() const { return content_; }

  string type() const { return content_.get("type", Json("")).as_string(); }

 private:
  Json content_;
};

#endif  // COMMUNICATION_MESSAGE_H_
