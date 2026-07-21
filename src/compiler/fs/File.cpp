#include "compiler/fs/File.hpp"

#include <utility>

namespace compiler::fs {
  File::File()
    : path{}, handle{compiler::utility::OS::InvalidNativeHandle}, file_id{} {}

  File::File(std::filesystem::path path)
    : path{std::move(path)}, handle{compiler::utility::OS::InvalidNativeHandle}, file_id{}
  {
    handle = compiler::utility::OS::OpenNativeFile(this->path);
    if (IsOpen()) {
      (void)compiler::utility::OS::GetFileID(handle, file_id.Data());
    }
  }

  File::File(File&& other) noexcept
    : path{std::move(other.path)}, handle{other.handle}, file_id{std::move(other.file_id)}
  {
    other.handle = compiler::utility::OS::InvalidNativeHandle;
    other.file_id = compiler::fs::ID{};
  }

  File& File::operator=(File&& other) noexcept {
    if (this != &other) {
      Close();
      path = std::move(other.path);
      handle = other.handle;
      file_id = std::move(other.file_id);
      other.handle = compiler::utility::OS::InvalidNativeHandle;
      other.file_id = compiler::fs::ID{};
    }

    return *this;
  }

  File::~File() {
    Close();
  }

  bool File::ReadToString(std::string& out) const {
    if (!IsOpen()) {
      return false;
    }

    std::string buffer;
    if (!compiler::utility::OS::ReadFileToString(handle, buffer)) {
      return false;
    }

    out = std::move(buffer);
    return true;
  }

  std::string File::ReadToString() const {
    std::string out;
    (void)ReadToString(out);
    return out;
  }

  void File::Close() {
    if (handle != compiler::utility::OS::InvalidNativeHandle) {
      compiler::utility::OS::CloseNativeFile(handle);
      handle = compiler::utility::OS::InvalidNativeHandle;
    }
  }
}