#include "communication/message.h"

Message::Message(const Json& content)
    : content_(content) {
}

Message::Message(const string& content)
    : content_(Json::parse_string(content)) {
}

string Message::ToString() const {
  return content_.as<string>() + "\n";
}
