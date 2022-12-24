#include "OperationListener.h"
#include "ConnectListener.hpp"

LedPi::OperationListener::OperationListener(ConnectListener &connectListener) : m_ConnectListener(connectListener)
{
}

void LedPi::OperationListener::connection_lost(const std::string &cause)
{
  spdlog::error("Connection lost to MQTT Client, cause: {0}", cause);
  m_ConnectListener.on_failure(mqtt::token(mqtt::token::Type::DISCONNECT, *m_ConnectListener.m_Client.get()));
}

void LedPi::OperationListener::message_arrived(mqtt::const_message_ptr msg)
{
}
