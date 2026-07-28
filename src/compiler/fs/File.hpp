#pragma once

#include <filesystem>
#include <string>

#include "compiler/utility/OS.hpp"
#include "compiler/fs/ID.hpp"

namespace compiler::fs {
  // Move-only file wrapper that holds a native handle and its path.
  class File {
  private:
    std::filesystem::path path;
    compiler::utility::OS::NativeHandle handle;
    compiler::fs::ID file_id;
  public:
    File();
    explicit File(std::filesystem::path path);

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    File(File&& other) noexcept;
    File& operator=(File&& other) noexcept;

    ~File();

    bool IsOpen() const { return handle != compiler::utility::OS::InvalidNativeHandle; }
    const std::filesystem::path& Path() const { return path; }
    compiler::utility::OS::NativeHandle NativeHandle() const { return handle; }
    [[nodiscard]] const compiler::fs::ID& ID() const { return file_id; }

    bool ReadToString(std::string& out) const;
    std::string ReadToString() const;
    void Close();

    explicit operator bool() const noexcept { return IsOpen(); }
  };
}
