#include "OperationListener.h"
#include "ConnectListener.hpp"
#include "command/StripCommandRegister.hpp"
#include <functional>
#include <spdlog/spdlog.h>

LedPi::OperationListener::OperationListener(ConnectListener &connectListener) : m_ConnectListener(connectListener)
{
  for(auto& el : connectListener.m_Strips) {
    auto cr = std::make_shared<StripCommandRegister>(el);
    std::function<void(std::string, std::string)> send = ([=](std::string key, std::string value) {
        spdlog::info("Sending message Topic: {0}/{1} Message: {2}", cr->GetPrefix(), key, value);
        if (m_ConnectListener.m_Client->is_connected())
        {
          m_ConnectListener.m_Client->publish(cr->GetPrefix() + "/" + key, value, 0, false);
        }
    });
    cr->RegisterPublishers(send);
    cr->RegisterCommands();
    m_CommandRegisters.push_back(cr);
  }

}

void LedPi::OperationListener::connection_lost(const std::string &cause)
{
  spdlog::error("Connection lost to MQTT Client, cause: {0}", cause);
  m_ConnectListener.on_failure(mqtt::token(mqtt::token::Type::DISCONNECT, *m_ConnectListener.m_Client.get()));
}

void LedPi::OperationListener::message_arrived(mqtt::const_message_ptr msg)
{
  spdlog::info("Recieved message Topic: {0} Message: {1}", msg->get_topic(), msg->get_payload());
  for(auto& el : m_CommandRegisters) {
    if (msg->get_topic().rfind(el->GetPrefix(), 0) == 0)
    {
      el->RunCommand(msg->get_topic(), msg->get_payload());
    }
  }
}

void LedPi::OperationListener::connected(const std::string &cause)
{
  spdlog::info("Connected now will subscribe to topics");
  for(auto& el : m_CommandRegisters) {
    for(auto& cn : el->GetCommandNames()) {
      this->m_ConnectListener.m_Client->subscribe(el->GetPrefix() + "/" + cn, 0);
    }
  }
}
