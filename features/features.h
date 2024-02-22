/*
* File and Directory Management: Handles operations such as cd, ls, cat, mkdir, mkfile, 
* and share within the constraints of the encrypted filesystem.
*/

#ifndef FEATURES_H
#define FEATURES_H

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <filesystem>
#include <fstream>

#include "helpers/helper_functions.h"
#include "encryption/randomizer_function.h"
#include "authentication/authentication.h"
#include "features_helpers.h"

void printDecryptedCurrentPath(std::string filesystemPath) {
  std::string pwd = decryptFilePath(getCustomPWD(filesystemPath), filesystemPath);
  std::cout << pwd << std::endl;
}

/**
 * Shows content of current directory
 * @param filesystemPath The base path of the filesystem
 */
void listDirectoryContents(std::string filesystemPath) {
    std::string path = fs::current_path();
    std::cout << "d -> ." << std::endl;

    if (path != filesystemPath + "/filesystem") {
        std::cout << "d -> .." << std::endl;
    }

    for (const fs::directory_entry& entry : fs::directory_iterator(path)) {
        std::string entryPath = entry.path().filename().string();

        if (entryPath.find(".") == 0) {
            continue;
        }

        fs::file_status status = entry.status();
        std::string decryptedName = FilenameRandomizer::DecryptFilename(entryPath, filesystemPath);

        if (status.type() == fs::file_type::directory) {
            std::cout << "d -> " << decryptedName << std::endl;
        } else if (status.type() == fs::file_type::regular) {
            std::cout << "f -> " << decryptedName << std::endl;
        }
    }
}

/**
 * Create a file with content.
 * @param filename The name of the file.
 * @param filepath The path where the file is located.
 * @param content Content of the file.
 */
void addContentsToFile(std::string filename, std::string filepath, std::string content) {
  filepath += "/" + filename;
  std::ofstream file(filepath, std::ios::app);

  if (file) {
    file << content << std::endl;
    file.close();
  } 
  else {
    std::ofstream newFile(filepath);
    if (newFile) {
        newFile << content << std::endl;
    }
    newFile.close();
  }
}

/**
 * Shares a file with a specified user by encrypting and copying it to the user's shared directory.
 * The function first checks if the file and the user exist. Then, it encrypts the file with the user's key
 * and places it in the user's shared directory, recording the action in a shared files log.
 * 
 * @param key The encryption key used for decrypting the file before re-encrypting it for the recipient.
 * @param username The name of the user with whom the file is to be shared.
 * @param filename The name of the file to share.
 * @param filesystemPath The base path of the filesystem where the file is located.
 * @param loggedUsername The username of the user who is sharing the file.
 */
void shareFile(std::vector<uint8_t> key, std::string username, std::string filename, std::string filesystemPath, std::string loggedUsername) {
    std::string randomizedFilename = FilenameRandomizer::GetRandomizedName(getCustomPWD(filesystemPath) + "/" + filename, filesystemPath);

    if (!doesFileExist(randomizedFilename) || !doesUserExist(username, filesystemPath)) {
        return;
    }

    std::string randomizedUserDirectory = getRandomizedUserDirectory(username, filesystemPath);
    std::string randomizedSharedDirectory = getRandomizedSharedDirectory(randomizedUserDirectory, filesystemPath);
    std::string content = Encryption::decryptFile(randomizedFilename, key);
    std::vector<uint8_t> shareKey = readEncKeyFromMetadata(username, filesystemPath + "/common/");
    std::string filenameKey = "/filesystem/" + randomizedUserDirectory + "/" + randomizedSharedDirectory + "/" + loggedUsername + "-" + filename;
    std::string sharedRandomizedFilename = FilenameRandomizer::EncryptFilename(filenameKey, filesystemPath);
    std::string shareUserPath = filesystemPath + "/filesystem/" + randomizedUserDirectory + "/" + randomizedSharedDirectory + "/" + sharedRandomizedFilename;
    Encryption::encryptFile(shareUserPath, content, shareKey);

    std::string sharedDataPath = filesystemPath + "/shared";
    std::string sharedDataContent = username + ":" + filenameKey;
    addContentsToFile(randomizedFilename, sharedDataPath, sharedDataContent);
    std::cout << "File shared successfully!" << std::endl;
}

/**
 * Shows file contents based on user access.
 *
 * @param inputStream Filename to access.
 * @param filesystemPath The base path of the filesystem.
 * @param userType User type.
 * @param key The encryption key used for decrypting the file content.
 */
void processFileAccess(std::istringstream& inputStream, std::string filesystemPath, UserType userType, std::vector<uint8_t> key) {
    std::string filename;
    inputStream >> filename;

    if (filename.empty()) {
        std::cout << "File name not provided" << std::endl;
        return;
    }
    if (filename.find('/') != std::string::npos) {
        std::cout << "File name cannot contain '/'" << std::endl;
        return;
    }

    std::string path = getCustomPWD(filesystemPath) + "/" + filename;
    std::string encryptedName = FilenameRandomizer::GetRandomizedName(path, filesystemPath);

    if (!fs::exists(encryptedName)) {
        std::cerr << "File does not exist" << std::endl;
        return;
    }
    if (fs::is_directory(fs::status(encryptedName))) {
        std::cerr << "File does not exist" << std::endl;
        return;
    }

    if (userType == UserType::admin) {
        std::string pwd = decryptFilePath(getCustomPWD(filesystemPath), filesystemPath);
        std::string userForKey = getUsernameFromPath(pwd);
        std::vector<uint8_t> userKey = readEncKeyFromMetadata(userForKey, filesystemPath + "/common/");
        std::cout << Encryption::decryptFile(encryptedName, userKey) << std::endl;
    } else {
        std::cout << Encryption::decryptFile(encryptedName, key) << std::endl;
    }
}

/**
 * Handles file sharing
 *
 * @param inputStream Contains filename and username with whom to share the file.
 * @param userName Source user.
 * @param key The encryption key for the file to be shared.
 * @param filesystemPath The base filesystem path.
 */
void handleFileSharing(std::istringstream& inputStream, std::string userName, std::vector<uint8_t> key, std::string filesystemPath) {
    std::string filename, shareUsername;
    inputStream >> filename >> shareUsername;

    if (!checkIfPersonalDirectory(userName, getCustomPWD(filesystemPath), filesystemPath)) {
        std::cout << "Forbidden" << std::endl;
        return;
    }

    if (filename.find('/') != std::string::npos) {
        std::cout << "File name cannot contain '/'" << std::endl;
        return;
    }

    if (isFileSharedWithUser(filename, filesystemPath, shareUsername, userName)) {
        std::cout << "A file with name " << filename << " has already been shared with " << shareUsername << std::endl;
    } else {
        shareFile(key, shareUsername, filename, filesystemPath, userName);
    }
}

/**
 * Create directory
 *
 * @param directoryName The name of the directory to create.
 * @param filesystemPath The base filesystem path.
 * @param username Username.
 */
void createDirectoryInUserSpace(std::string directoryName, std::string &filesystemPath, std::string username) {
  if (!checkIfPersonalDirectory(username, getCustomPWD(filesystemPath), filesystemPath)) {
    std::cerr << "Forbidden" << std::endl;
    return;
  }

  if (directoryName.find('/') != std::string::npos) {
    std::cout << "Directory name cannot contain '/'" << std::endl;
    return;
  }

  std::string path = getCustomPWD(filesystemPath) + "/" + directoryName;
  std::string encryptedName = getEncFilename(directoryName, path, filesystemPath, true);
  if (!encryptedName.empty()) {
    system(("mkdir -p " + encryptedName).c_str());
    std::cout << "Directory created successfully." << std::endl;
  }
}

/**
 * Handles the creation of a new directory.
 *
 * @param directoryName Directory name.
 * @param filesystemPath The base path of the filesystem.
 * @param userName Username.
 */
void processCreateDirectoryInUserSpace(std::string directoryName, std::string filesystemPath, std::string userName) {
    if (directoryName.find('/') != std::string::npos || directoryName.find('`') != std::string::npos) {
        std::cerr << "Directory name cannot contain '/' or '`'" << std::endl;
        return;
    }
    if (!checkIfPersonalDirectory(userName, getCustomPWD(filesystemPath), filesystemPath)) {
        std::cout << "Forbidden: User lacks permission to create directory here." << std::endl;
        return;
    }
    if (directoryName.empty() || directoryName == "filesystem" || directoryName == "." || directoryName == "..") {
        std::cerr << "Invalid directory name provided." << std::endl;
        return;
    }
    
    fs::path targetPath = fs::absolute(directoryName);
    if (fs::exists(targetPath) && fs::is_directory(targetPath)) {
        std::cerr << "Directory already exists." << std::endl;
        return;
    }
    try {
        createDirectoryInUserSpace(directoryName, filesystemPath, userName);
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Failed to create directory: " << e.what() << std::endl;
    }
}

/**
 * Creates new file
 *
 * @param inputStream The input stream to extract the filename and contents from.
 * @param userName The name of the user attempting to create the file.
 * @param key The encryption key for the file.
 * @param filesystemPath The base path of the filesystem.
 */
void processFileCreation(std::istringstream& inputStream, std::string userName, std::vector<uint8_t> key, std::string filesystemPath) {
    std::string filename, contents;
    inputStream >> filename;
    std::getline(inputStream, contents);

    if (filename.find('/') != std::string::npos) {
        std::cout << "File name cannot contain '/'" << std::endl;
        return;
    }
    if (!checkIfPersonalDirectory(userName, getCustomPWD(filesystemPath), filesystemPath)) {
        std::cout << "Forbidden" << std::endl;
        return;
    }

    std::filesystem::path pathObj(filename);
    std::string filenameStr = pathObj.filename().string();
    if (!filenameStr.empty() && isValidFilename(filename)) {
        createAndEncryptFile(filename, contents, key, filesystemPath, userName);
    } else {
        std::cerr << "Not a valid filename, try again." << std::endl;
    }
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
    } else if (user_type == user) {
        std::cout << "++++++++++++++++++++++++" << std::endl;
        // set root path = user's root path which is its own directory
        std::string user_folder = FilenameRandomizer::GetRandomizedName("/filesystem/" + user_name, filesystem_path);
        rootPath = userRootPath / user_folder;
    }

    fs::current_path(rootPath);
    std::string input_feature, cmd, filename, username, directory_name, contents;

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
        } else if (cmd == "ls") {
            listDirectoryContents(filesystem_path);
        } else if (cmd == "cat") {
            processFileAccess(istring_stream, filesystem_path, user_type, key);
        } else if (cmd == "share") {
            handleFileSharing(istring_stream, user_name, key, filesystem_path);
        } else if (cmd == "mkdir") {
            istring_stream >> directory_name;
            processCreateDirectoryInUserSpace(directory_name, filesystem_path, user_name);
        } else if (cmd == "mkfile") {
            processFileCreation(istring_stream, user_name, key, filesystem_path);
        }
    } while (cmd != "exit"); // only exit out of command line when using "exit" cmd

    return 1;
}

#endif // FEATURES_H
