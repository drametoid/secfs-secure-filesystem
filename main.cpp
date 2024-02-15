#include <iostream>
#include <filesystem>

#include "authentication/authentication.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  std::string filesystemPath = fs::current_path();

  if(fs::exists("filesystem")) {
    if(argc != 2) {
      std::cout << "Invalid keyfile\n" << std::endl;
      return 1;
    }
    else {
      std::string keyFileName = argv[1];
      std::string userName = "admin";
      UserType userType;
      if(userName == "admin")
        userType = admin;
      else
        userType = user;
    }
  }
}
