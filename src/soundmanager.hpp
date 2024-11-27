#pragma once

#include "common.hpp"
#include "soundfx.hpp"

namespace audio {
class soundmanager {
public:
  soundmanager(std::shared_ptr<audiodevice> audiodevice) noexcept;
  ~soundmanager() noexcept = default;

  std::shared_ptr<soundfx> get(std::string_view filename);
  void play(std::string_view filename) noexcept;
  void stop(std::string_view filename) noexcept;
  void flush() noexcept;

private:
  std::shared_ptr<audiodevice> _audiodevice;
  std::map<std::string_view, std::shared_ptr<soundfx>> _soundmap;
};
}
