#pragma once

#include <string>

namespace LedPi {
  class MqttConfig {
    public:
      std::string host;
      int port = 1883;
  }; 
  class NetworkConfig {
    public:
      int reactivePort;
      MqttConfig mqtt;
  };
}