#pragma once

#include "mqtt/async_client.h"

#include "../../strip/IStrip.h"
#include "../../effects/EffectRegistry.hpp"
#include <timercpp/timercpp.h>
#include <spdlog/spdlog.h>
#include <map>
#include <cstdint>

#include "OperationListener.h"

namespace LedPi
{
  class MQTTClient;
  class ConnectListener : public mqtt::iaction_listener
  {
  public:
    ConnectListener(std::shared_ptr<mqtt::async_client> client,
                    mqtt::connect_options connectOptions,
                    std::vector<std::shared_ptr<LedPi::IStrip>> &strips,
                    MQTTClient &clientImpl) : m_Client(client), m_ConnectOptions(connectOptions),
                                              m_Strips(strips), m_ClientImpl(clientImpl) {}

    virtual ~ConnectListener() {}

    virtual void on_failure(const mqtt::token &asyncActionToken) override
    {
      m_ConnectOptions.set_connect_timeout(1);
      m_ConnectOptions.set_automatic_reconnect(false);
      m_ConnectOptions.set_clean_session(true);

      spdlog::error("Failed to connect to the MQTT Server Retrying in 5 seconds! Attempts made: {0}", connectAttempts);

      for (auto &el : m_Strips)
      {
        IEffect *e = el->GetEffect().get();
        if (e->GetName() == "connecting")
        {
          Effects::Connecting *c = (Effects::Connecting *)(e);
          c->Fail();
        } else {
          m_PreviousBrightness[el] = el->GetBrightness();
          m_PreviousEffect[el] = el->GetEffect();
          el->GetEffect()->Stop();
          el->SetEffect(Effect::Connecting, {});
          el->SetBrightness(255);
          Effects::Connecting *c = (Effects::Connecting *)(el->GetEffect().get());
          c->Fail();
        }
        
        m_Timer.setTimeout([&]()
                           {
              if (!m_Client->is_connected())
              {
                try {
                 connectAttempts++;
                 m_Client->connect(m_ConnectOptions, nullptr, *this);
                } catch(std::exception& e) {
                  spdlog::error("Something went wrong {0}", e.what());
                }
              }
                            
        }, 5000);
      }
    };

    virtual void on_success(const mqtt::token &asyncActionToken) override
    {
      spdlog::info("Connected to mqtt server");
      for (auto &el : m_Strips)
      {
        IEffect *e = el->GetEffect().get();
        Effects::Connecting *c = (Effects::Connecting *)(e);
        c->Success();
        m_Timer.setTimeout([&]()
                           {
                            
          el->GetEffect()->Stop();
          if (m_PreviousEffect.count(el))
          {
            el->SetEffect(EffectRegistry->GetEffectFromName(m_PreviousEffect[el]->GetName()), m_PreviousEffect[el]->GetConfig());
            el->SetBrightness(m_PreviousBrightness[el]);
            m_PreviousEffect.erase(el);
            m_PreviousBrightness.erase(el);
          } else {
            el->SetEffect(Effect::Solid, {});
            el->SetBrightness(150); 
          }
          
          }, 350 * 5);

          connectAttempts = 0;
          if (!m_OperationListener)
          {
            m_OperationListener = std::make_shared<OperationListener>(*this);
          }
          m_Client->set_callback(*m_OperationListener.get());
      }
    };

  public:
    std::shared_ptr<mqtt::async_client> m_Client;
    mqtt::connect_options m_ConnectOptions;
    std::vector<std::shared_ptr<LedPi::IStrip>> &m_Strips;
    MQTTClient &m_ClientImpl;
    uint8_t connectAttempts = 1;
    Timer m_Timer;
    std::shared_ptr<OperationListener> m_OperationListener;
    std::map<std::shared_ptr<LedPi::IStrip>, uint8_t> m_PreviousBrightness;
    std::map<std::shared_ptr<LedPi::IStrip>, std::shared_ptr<LedPi::IEffect>> m_PreviousEffect;
  };
}