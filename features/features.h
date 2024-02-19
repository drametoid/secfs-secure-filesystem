/*
* File and Directory Management: Handles operations such as cd, ls, cat, mkdir, mkfile, 
* and share within the constraints of the encrypted filesystem.
*/

#ifndef FEATURES_H
#define FEATURES_H

#include <iostream>

#include "authentication/authentication.h"

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
    return 1;
}

#endif // FEATURES_H
