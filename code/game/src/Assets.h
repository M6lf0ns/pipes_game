#pragma once

#include <filesystem>

/// @brief Macro to convert a preprocessor definition to a string literal.
#define GET_STRING(x) #x
/// @brief Macro to resolve the directory string.
#define GET_DIR(x) GET_STRING(x)

namespace fs = std::filesystem;

namespace gl3 {
    /// @brief Resolves a relative asset path to an absolute path based on the ASSET_ROOT definition.
    ///
    /// This utility function uses the preprocessor definition 'ASSET_ROOT' to construct
    /// the full file system path. It ensures the path uses the platform-preferred
    /// directory separators.
    ///
    /// @param relativeAssetPath The path relative to the defined asset root.
    /// @return An absolute std::filesystem::path pointing to the asset.
    inline fs::path resolveAssetPath(const fs::path &relativeAssetPath) {
        auto mergedPath = (fs::path(GET_DIR(ASSET_ROOT)) / relativeAssetPath).make_preferred();
        return mergedPath; // no canonical, no crash
    }
}