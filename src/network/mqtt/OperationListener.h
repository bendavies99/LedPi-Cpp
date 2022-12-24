#pragma once

#include "mqtt/async_client.h"
#include <spdlog/spdlog.h>
#include <cstdint>
#include "ConnectListener_fwd.h"

namespace LedPi
{
  class OperationListener : public mqtt::callback
  {
  public:
    OperationListener(ConnectListener& connectListener);    
    virtual void connection_lost(const std::string &cause) override;
    virtual void message_arrived(mqtt::const_message_ptr msg) override;

  private:
    ConnectListener& m_ConnectListener;
  };
}