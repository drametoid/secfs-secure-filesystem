/*
* Helper functions needed across the code.
*/

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

/// Helper function to create a new directory
/// \param path Path for creating dir
bool createDirectory(const fs::path& path) {
    std::error_code ec;
    if (fs::create_directory(path, ec)) {
        return true;
    } else {
        if (ec) {
            std::cerr << "Error creating directory: " << ec.message() << " (" << path << ")" << std::endl;
            return false;
        } else {
            std::cerr << "Directory already exists: " << path << std::endl;
            return true;
        }
    }
}

std::vector<uint8_t> readEncKeyFromMetadata(const std::string& userName, const std::string& directory) {
    const std::string metadataFilePath = !directory.empty() ? directory : "common/";
    std::ifstream metadataFile(metadataFilePath + userName + "_key", std::ios::in | std::ios::binary);

    if (!metadataFile) {
        std::cerr << "Failed to read key from metadata for " << userName << std::endl;
        return {}; // Return an empty vector if the file failed to open
    }

    std::vector<uint8_t> encryptionKey(KEY_SIZE);
    metadataFile.read(reinterpret_cast<char*>(encryptionKey.data()), encryptionKey.size());

    return encryptionKey;
}


#endif // HELPER_FUNCTIONS_H
