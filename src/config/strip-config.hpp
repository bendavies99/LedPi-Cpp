#pragma once

#include <string>

namespace LedPi { 
  class StripConfig {
    public:
      std::string name;
      uint16_t ledCount;
      uint8_t pinNumber;
      uint8_t uid;
  };
}