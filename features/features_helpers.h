/*
* Helper functions needed for file operations/features.
*/

#ifndef FEATURES_HELPERS_H
#define FEATURES_HELPERS_H

#include <iostream>
#include <filesystem>
#include <vector>

#include "encryption/randomizer_function.h"

namespace fs = std::filesystem;

fs::path adminRootPath = fs::current_path() / "filesystem";
fs::path userRootPath = fs::current_path() / "filesystem";
fs::path rootPath;

std::string getCustomPWD(const std::string& basePath) {
    std::string currentPath = fs::current_path().string();
    return currentPath.erase(1, basePath.length());
}

// Helper function to process the path and extract/decrypt filenames
std::vector<std::string> processAndDecryptPath(std::string path, const std::string& filesystemPath) {
    const std::string delimiter = "/";
    std::vector<std::string> filenames;
    size_t pos = 0;

    while ((pos = path.find(delimiter)) != std::string::npos) {
        std::string name = path.substr(0, pos);
        if (name == "filesystem") {
            // Add 'filesystem' directly without decryption
            filenames.push_back(name);
        } else {
            // Decrypt the name and add it to the filenames vector
            std::string decrypted = FilenameRandomizer::GetFilename(name, filesystemPath);
            filenames.push_back(decrypted);
        }
        // Prepare for the next iteration
        path.erase(0, pos + delimiter.length());
    }

    // Handle the case where there is a remaining part of the path after the last delimiter
    if (!path.empty()) {
        if (path == "filesystem") {
            filenames.push_back(path);
        } else {
            std::string decrypted = FilenameRandomizer::GetFilename(path, filesystemPath);
            filenames.push_back(decrypted);
        }
    }

    return filenames;
}

std::string decryptFilePath(std::string path, const std::string& filesystemPath) {
    // Normalize the path by removing a leading "/"
    if (!path.empty() && path[0] == '/') {
        path = path.substr(1);
    }

    std::vector<std::string> decryptedFilenames = processAndDecryptPath(path, filesystemPath);

    // Reconstruct the decrypted file path
    std::string decryptedFilePath = "/";
    for (const auto& name : decryptedFilenames) {
        decryptedFilePath += name + "/";
    }
    decryptedFilePath.pop_back(); // Remove the trailing "/"

    return decryptedFilePath;
}

#endif // FEATURES_HELPERS_H
