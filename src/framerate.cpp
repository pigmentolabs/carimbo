#include "framerate.hpp"

using namespace framework;

uint64_t framerate::per_second() const noexcept {
  return _frames;
}

void framerate::loop(float_t delta) noexcept {
  UNUSED(delta);
  _frames++;

  const auto now = SDL_GetTicks();
  _elapsed += now - _start;
  _start = now;

  if (_elapsed >= 1000) {
    std::cout << std::fixed << std::setprecision(1) << (_frames / (_elapsed / 1000.0f)) << std::endl;
    _elapsed = 0;
    _frames = 0;
  }
}
