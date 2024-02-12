#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  std::string filesystemPath = fs::current_path();

  if(fs::exists("filesystem")) {
    if(argc != 2) {
      std::cout << "Invalid keyfile\n" << std::endl;
      return 1;
    }
  }
}
