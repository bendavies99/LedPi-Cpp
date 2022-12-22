#pragma once

#include <string>

namespace LedPi { 
  class ApplicationConfig {
    public:
      std::string applicationName;
      bool debugMode;
      int debugPort = 27321;
  };
}