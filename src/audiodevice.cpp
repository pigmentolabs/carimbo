#include "audiodevice.hpp"

using namespace audio;

audiodevice::audiodevice() : device(nullptr, [](ALCdevice *device) {
                               if (device) {
                                 alcCloseDevice(device);
                               }
                             }),
                             context(nullptr, [](ALCcontext *context) {
                               if (context) {
                                 alcDestroyContext(context);
                               }
                             }) {

  device.reset(alcOpenDevice(nullptr));
  if (!device) {
    throw std::runtime_error("Failed to open ALC device");
  }

  context.reset(alcCreateContext(device.get(), nullptr));
  if (!context) {
    throw std::runtime_error("Failed to create ALC context");
  }

  alcMakeContextCurrent(context.get());
}

audiodevice::~audiodevice() {
  std::cout << ">>> audiodevice::~audiodevice()" << std::endl;
  if (context) {
    alcMakeContextCurrent(nullptr);
    std::cout << "ALC context unset successfully." << std::endl;
  }
}
