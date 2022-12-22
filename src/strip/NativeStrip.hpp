#pragma once

#include "BaseStrip.h"
#include <spdlog/spdlog.h>

namespace LedPi {
  class NativeStrip: public BaseStrip {
    public:
      NativeStrip(const StripConfig& stripConfig): BaseStrip(stripConfig) {

      }

      void Render() override {};
  };
}