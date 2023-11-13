#include "io.hpp"

const std::vector<uint8_t> io::read(std::string_view filename) {
  const auto fp = PHYSFS_openRead(filename.data());
  if (fp == nullptr) {
    auto files = PHYSFS_enumerateFiles("/");

    if (files != nullptr) {
      for (auto i = files; *i != nullptr; i++) {
        std::cout << *i << std::endl;
      }
      PHYSFS_freeList(files);
    }

    throw std::runtime_error(fmt::format("[PHYSFS_openRead] error while opening file: {}, error: {}", filename, PHYSFS_getErrorByCode(PHYSFS_getLastErrorCode())));
  }

  PHYSFS_sint64 length = PHYSFS_fileLength(fp);
  std::vector<uint8_t> buffer(length);
  PHYSFS_readBytes(fp, &buffer[0], length);
  PHYSFS_close(fp);

  return buffer;
}
