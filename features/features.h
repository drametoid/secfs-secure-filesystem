/*
* File and Directory Management: Handles operations such as cd, ls, cat, mkdir, mkfile, 
* and share within the constraints of the encrypted filesystem.
*/

#ifndef FEATURES_H
#define FEATURES_H

#include <iostream>
#include <filesystem>

#include "authentication/authentication.h"
#include "features_helpers.h"

void printDecryptedCurrentPath(std::string filesystemPath) {
  std::string pwd = decryptFilePath(getCustomPWD(filesystemPath), filesystemPath);
  std::cout << pwd << std::endl;
}

int userFeatures(std::string user_name, UserType user_type, std::vector<uint8_t> key, std::string filesystem_path) {
    std::cout << "++++++++++++++++++++++++" << std::endl;
    std::cout << "++| WELCOME TO EFS! |++" << std::endl;
    std::cout << "++++++++++++++++++++++++" << std::endl;
    std::cout << "\nEFS Commands Available: \n" << std::endl;

    std::cout << "cd <directory> \n"
            "pwd \n"
            "ls  \n"
            "cat <filename> \n"
            "share <filename> <username> \n"
            "mkdir <directory_name> \n"
            "mkfile <filename> <contents> \n"
            "exit \n";
    if (user_type == admin) {
        // if admin, allow the following command as well
        std::cout << "adduser <username>" << std::endl;
        std::cout << "++++++++++++++++++++++++" << std::endl;
    }

    do {
        std::cout << user_name << " " << decryptFilePath(getCustomPWD(filesystem_path), filesystem_path) << "> ";
        // get command from the user
        getline(std::cin, input_feature);

        if (std::cin.eof()) {
            std::cout << "Ctrl+D detected." << std::endl;
            return 1;
        }

        // get the first word (command) from the input
        std::istringstream istring_stream(input_feature);
        istring_stream >> cmd;

        if (cmd == "pwd") {
            printDecryptedCurrentPath(filesystem_path);
        }
    } while (cmd != "exit"); // only exit out of command line when using "exit" cmd

    return 1;
}

#endif // FEATURES_H
