std::pair<std::vector<uint8_t>, geometry::size> _load_png(std::string_view filename) {
  const auto buffer = storage::io::read(filename);

  auto ctx = std::unique_ptr<spng_ctx, decltype(&spng_ctx_free)>(spng_ctx_new(0), spng_ctx_free);

  if (const auto error = spng_set_png_buffer(ctx.get(), buffer.data(), buffer.size()); error != SPNG_OK) [[unlikely]] {
    std::ostringstream oss;
    oss << "[spng_set_png_buffer] error while parsing image: " << filename
        << ", error: " << spng_strerror(error);
    throw std::runtime_error(oss.str());
  }

  spng_ihdr ihdr{};
  if (const auto error = spng_get_ihdr(ctx.get(), &ihdr); error != SPNG_OK) [[unlikely]] {
    std::ostringstream oss;
    oss << "[spng_get_ihdr] error while getting image information: " << filename
        << ", error: " << spng_strerror(error);
    throw std::runtime_error(oss.str());
  }

  const int format{SPNG_FMT_RGBA8};
  size_t length{0};
  if (const auto error = spng_decoded_image_size(ctx.get(), format, &length); error != SPNG_OK) [[unlikely]] {
    std::ostringstream oss;
    oss << "[spng_decoded_image_size] error while getting image size: " << filename
        << ", error: " << spng_strerror(error);
    throw std::runtime_error(oss.str());
  }

  std::vector<uint8_t> output(length);
  if (const auto error = spng_decode_image(ctx.get(), output.data(), length, format, SPNG_DECODE_TRNS); error != SPNG_OK) [[unlikely]] {
    std::ostringstream oss;
    oss << "[spng_decode_image] error while decoding image: " << filename
        << ", error: " << spng_strerror(error);
    throw std::runtime_error(oss.str());
  }

  const auto size = geometry::size{static_cast<int32_t>(ihdr.width), static_cast<int32_t>(ihdr.height)};

  return std::make_pair(output, size);
}
