#pragma once

#include "mqtt/async_client.h"

#include "../../strip/IStrip.h"
#include "../../effects/EffectRegistry.hpp"
#include <timercpp/timercpp.h>
#include <spdlog/spdlog.h>

namespace LedPi
{
  class MQTTClient;
  class ConnectListener : public mqtt::iaction_listener
  {
  public:
    ConnectListener(std::shared_ptr<mqtt::async_client> client, 
    mqtt::connect_options connectOptions, 
    std::vector<std::unique_ptr<LedPi::IStrip>>& strips,
    MQTTClient& clientImpl): m_Client(client), m_ConnectOptions(connectOptions), 
    m_Strips(strips), m_ClientImpl(clientImpl) {}

    virtual ~ConnectListener() {}

    virtual void on_failure(const mqtt::token &asyncActionToken) override{
      spdlog::error("Failed to connect!");
    };

    virtual void on_success(const mqtt::token &asyncActionToken) override{
      for(auto& el : m_Strips) {
        IEffect* e = el->GetEffect().get();
        Effects::Connecting* c = (Effects::Connecting*)(e);
        c->Success();
        timer.setTimeout([&]() {
          el->SetEffect(Effect::Solid, {});
          el->SetBrightness(150);
        }, 350 * 5);
      }
    };

  private:
    std::shared_ptr<mqtt::async_client> m_Client;
    mqtt::connect_options m_ConnectOptions;
    std::vector<std::unique_ptr<LedPi::IStrip>>& m_Strips;
    MQTTClient& m_ClientImpl;
    uint8_t connectAttempts = 1;
    Timer timer;
  };
}