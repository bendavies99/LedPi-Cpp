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

std::string gen_random(const int len) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    std::string tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

namespace LedPi
{
  class MQTTClient
  {
  public:
    MQTTClient(std::shared_ptr<NetworkConfig> networkConfig, std::vector<std::shared_ptr<LedPi::IStrip>> &strips)
    {
      try
      {
        std::string client = "LedPi-Client-" + gen_random(10);
        auto &mqtt = networkConfig->mqtt;
        m_Client = std::make_shared<mqtt::async_client>("tcp://" + mqtt.host + ":" + std::to_string(mqtt.port), client);
        auto connOpts = mqtt::connect_options_builder()
                            .clean_session()
                            .connect_timeout(std::chrono::milliseconds(5000))
                            .automatic_reconnect(true)
                            .keep_alive_interval(std::chrono::milliseconds(10000))
                            .finalize();

        m_ConnectListener = std::make_shared<ConnectListener>(m_Client, connOpts, strips, *this);
        auto l = (mqtt::iaction_listener *)m_ConnectListener.get();
        spdlog::info("Connecting to the mqtt server with client name {0}", client);
        m_Client->connect(connOpts, nullptr, (*l));
      }
      catch (const std::exception &e)
      {
        spdlog::error("An Error occurred {0}", e.what());
        exit(1);
      }
    }
    void Shutdown()
    {
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