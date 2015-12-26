#include "communication/message.h"

Message::Message(const jsoncons::json& content)
    : content_(content) {
}

Message::Message(const string& content)
    : content_(jsoncons::json::parse_string(content)) {
}

string Message::ToString() const {
  return content_.as<string>() + "\n";
}
