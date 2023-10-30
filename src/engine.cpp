#include "engine.hpp"

engine::engine() : _running(false) {
}

void engine::create_window(std::string_view title, const uint32_t width, const uint32_t height) {
  _window = std::make_shared<window>(title, width, height);
  _renderer = _window->create_renderer();
}

void engine::add_loopable(const loopable &loopable, const uint64_t priority) {
  //
}

void engine::run() {
  _running = true;

  SDL_Event event;
  while (_running) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        _running = false;
      }
    }

    _renderer->begin_draw();

    _renderer->end_draw();
  }
}