#pragma once

#include "common.hpp"

namespace graphics {
using pixmap_ptr = std::shared_ptr<pixmap>;

class pixmappool {
public:
  explicit pixmappool(const std::shared_ptr<renderer> &renderer) noexcept;
  ~pixmappool() noexcept = default;

  const std::shared_ptr<pixmap> get(std::string_view filename);

  void flush() noexcept;

private:
  std::shared_ptr<renderer> _renderer;
  std::unordered_map<std::string_view, pixmap_ptr> _pool;
};
}
