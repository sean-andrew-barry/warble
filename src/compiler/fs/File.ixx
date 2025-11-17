export module compiler.fs.File;

import compiler.utility.OS;
import compiler.fs.ID;

import <filesystem>;
import <string>;
import <string_view>;
import <utility>;

namespace compiler::fs {
  // Move-only file wrapper that holds a native handle and its path.
  export class File {
  private:
    std::filesystem::path path;
    compiler::utility::OS::NativeHandle handle;
    compiler::fs::ID file_id;
  public:
    File() : path{}, handle{compiler::utility::OS::InvalidNativeHandle}, file_id{} {}

    explicit File(std::filesystem::path p)
      : path{std::move(p)}, handle{compiler::utility::OS::InvalidNativeHandle}, file_id{}
    {
      handle = compiler::utility::OS::OpenNativeFile(path);
      if (IsOpen()) {
        (void)compiler::utility::OS::GetFileID(handle, file_id.Data());
      }
    }

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    File(File&& other) noexcept
      : path{std::move(other.path)}, handle{other.handle}, file_id{std::move(other.file_id)}
    {
      other.handle = compiler::utility::OS::InvalidNativeHandle;
      other.file_id = compiler::fs::ID{};
    }

    File& operator=(File&& other) noexcept {
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

    ~File() { Close(); }

    bool IsOpen() const { return handle != compiler::utility::OS::InvalidNativeHandle; }
    const std::filesystem::path& Path() const { return path; }
    compiler::utility::OS::NativeHandle NativeHandle() const { return handle; }
    [[nodiscard]] const compiler::fs::ID& ID() const { return file_id; }

    bool ReadToString(std::string& out) const {
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

    std::string ReadToString() const {
      std::string out;
      (void)ReadToString(out);
      return out;
    }

    void Close() {
      if (handle != compiler::utility::OS::InvalidNativeHandle) {
        compiler::utility::OS::CloseNativeFile(handle);
        handle = compiler::utility::OS::InvalidNativeHandle;
      }
    }

    explicit operator bool() const noexcept { return IsOpen(); }
  };
}
