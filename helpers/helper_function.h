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

#endif // HELPER_FUNCTIONS_H
