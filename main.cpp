#include <cstdlib>
#include <iostream>

#include "gflags/gflags.h"

DEFINE_int32(port, 8091, "Port to start a server on.");

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  try {
    printf("Starting server on port %d.\n", FLAGS_port);
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
