#pragma once

#include "mqtt/async_client.h"
#include <string>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "../config/config.h"
#include <chrono>
#include <spdlog/spdlog.h>
#include "mqtt/ConnectListener.hpp"

namespace LedPi
{
  class MQTTClient
  {
  public:
    MQTTClient(std::shared_ptr<NetworkConfig> networkConfig, std::vector<std::unique_ptr<LedPi::IStrip>>& strips)
    {
      srand(time(0));
      std::string client = "LedPi-Client-" + (rand() % 20345);
      auto &mqtt = networkConfig->mqtt;
      m_Client = std::make_shared<mqtt::async_client>("tcp://" + mqtt.host + ":" + std::to_string(mqtt.port), client);
      auto connOpts = mqtt::connect_options_builder()
                          .clean_session()
                          .connect_timeout(std::chrono::milliseconds(5000))
                          .automatic_reconnect(true)
                          .keep_alive_interval(std::chrono::milliseconds(10000))
                          .finalize();

      m_ConnectListener = std::make_shared<ConnectListener>(m_Client, connOpts, strips, *this);
      auto l = (mqtt::iaction_listener*)m_ConnectListener.get();
      try
      {
        spdlog::info("Connecting to the mqtt server");
        m_Client->connect(connOpts, nullptr, (*l));
      }
      catch (const mqtt::exception &e)
      {
        spdlog::error("An Error occurred {0}", e.to_string());
        exit(1);
      }
    }
    void Shutdown() {
      m_Shutdown = true;
      if (m_Client->is_connected())
      {
        m_Client->disconnect()->wait();
      }
    }
  private:
    std::shared_ptr<mqtt::async_client> m_Client;
    std::shared_ptr<ConnectListener> m_ConnectListener;
    bool m_Shutdown = false;
  };
}