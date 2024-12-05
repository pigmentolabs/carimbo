#include "socketio.hpp"

using namespace network;

using json = nlohmann::json;

socketio::socketio() {
  _queue.reserve(10);

  const auto url = fmt::format("ws://{}:3000", emscripten_run_script_string("window.location.hostname"));

  EmscriptenWebSocketCreateAttributes attrs = {
      url.c_str(),
      nullptr,
      true
  };

  _socket = emscripten_websocket_new(&attrs);

  emscripten_websocket_set_onopen_callback(
      _socket,
      this,
      [](int, const EmscriptenWebSocketOpenEvent *event, void *data) noexcept {
        UNUSED(event);
        auto *self = static_cast<socketio *>(data);

        self->_connected = true;

        for (const auto &message : self->_queue) {
          self->send(message);
        }
        self->_queue.clear();

        self->invoke("connect");

        return 0;
      }
  );

  emscripten_websocket_set_onmessage_callback(
      _socket,
      this,
      [](int, const EmscriptenWebSocketMessageEvent *event, void *data) noexcept {
        if (!event->isText) [[unlikely]] {
          return 0;
        }

        auto *self = static_cast<socketio *>(data);
        const auto buffer = std::string(reinterpret_cast<const char *>(event->data), event->numBytes - 1);
        const auto j = json::parse(buffer, nullptr, false);
        if (j.is_discarded()) [[unlikely]] {
          return 0;
        }

        const auto it = j.find("command");
        if (it != j.end() && it->get<std::string>() == "ping") {
          self->send(R"({"command": "pong"})");
          return 0;
        }

        if (const auto &event = j.value("event", json::object()); !event.empty()) {
          self->invoke(
              event.at("topic").get_ref<const std::string &>(),
              event.at("data").dump()
          );

          return 0;
        }

        return 0;
      }
  );

  emscripten_websocket_set_onerror_callback(
      _socket,
      this,
      [](int, const EmscriptenWebSocketErrorEvent *event, void *data) noexcept {
        UNUSED(event);
        const auto self = static_cast<socketio *>(data);
        self->invoke("error", "WebSocket error occurred");
        return 0;
      }
  );

  emscripten_websocket_set_onclose_callback(
      _socket,
      this,
      [](int, const EmscriptenWebSocketCloseEvent *event, void *data) noexcept {
        UNUSED(event);
        const auto *self = static_cast<socketio *>(data);
        self->invoke("disconnect");
        return 0;
      }
  );
}

socketio::~socketio() {
  constexpr int code = 1000;
  constexpr const char *reason = "Client disconnecting";

  if (_socket) {
    emscripten_websocket_close(_socket, code, reason);
    emscripten_websocket_delete(_socket);
    _socket = 0;
    invoke("disconnect");
  }
}

void socketio::emit(const std::string &topic, const std::string &data) {
  send(fmt::format(R"({{"event":{{"topic":"{}","data":{}}}}})", topic, data));
}

void socketio::on(const std::string &topic, std::function<void(const std::string &)> callback) {
  send(fmt::format(R"({{"subscribe":"{}"}})", topic));
  _callbacks[topic].push_back(std::move(callback));
}

void socketio::send(const std::string &message) {
  if (!_connected) {
    _queue.emplace_back(message);
    return;
  }

  emscripten_websocket_send_utf8_text(_socket, message.c_str());
}

void socketio::invoke(const std::string &event, const std::string &data) const {
  if (auto it = _callbacks.find(event); it != _callbacks.end()) {
    for (const auto &callback : it->second) {
      callback(data);
    }
  }
}
